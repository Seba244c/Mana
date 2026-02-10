#include "Application.h"

#include "Mana/Audio/Audio.h"
#include "Mana/Core/Asset.h"
#include "Mana/Core/Core.h"
#include "Mana/Core/Log.h"
#include "Mana/Core/Platform.h"
#include "Mana/Events/Event.h"
#include "Mana/Events/IOEvents.h"
#include "Mana/Graphics/GraphicsAPI.h"
#include "Mana/IO/Input.h"
#include "Mana/IO/Window.h"
#include "Mana/Rendering/UI.h"
#include "Mana/Services/Service.h"
#include "Mana/World/ECS.h"

namespace Mana {
Application *Application::m_Instance;

int StartApplication(Application *app) {
    Mana::Log::Init();
    MC_INFO("Starting Application!");

    const AppInfo appInfo = app->GetAppInfo();
    MC_INFO("Name: {}", appInfo.Name);
    MC_INFO("Author: {}", appInfo.Author);
    MC_INFO("Version: {} ({})", appInfo.Version, appInfo.Build);
    MC_INFO("Build-type: {}", ISDEBUG ? "Debug" : "Release");
    if constexpr (ISHEADLESS)
        MC_WARN("Running in headless mode!");

    app->Start();

// We always need to call SDL_Quit even though we might've shut down the
// subsystems we ended up using
#ifdef MANAB_SDL
    SDL_Quit();
#endif

    delete app;
    return 0;
}

void Application::Start() {
    // Instance
    if (!m_Instance)
        m_Instance = this;

    // Init platform
    Platform::Init();

    // Since flecs systems are very central to the way the engine works, we load
    // that early
    ECS::Init(m_World);

    // Creating a shit ton of different things
    m_Services = CreateScope<ServicesManager>();
    m_Audio = CreateScope<AudioManager>();
    std::vector<std::filesystem::path> sourceFolders;
    if constexpr (ISDEBUG) {
        sourceFolders = m_AppConfig.Assets_SourceFolders;

        if (std::filesystem::exists(MANA_DEV_ASSETS_FOLER))
            sourceFolders.push_back(MANA_DEV_ASSETS_FOLER);
        else {
            MC_WARN("MANA_DEV_ASSETS_FOLDER ({}) was not found.",
                    MANA_DEV_ASSETS_FOLER);
            MC_WARN("This is only an issue if the developer didn't provide "
                    "precompiled motes,");
            MC_WARN("Or if you are said developer.");
        }
    }
    m_Assets =
        CreateScope<AssetManager>("./MoteCache/", sourceFolders, m_Services);

    // Window with a default title
    if (m_AppConfig.Input_WindowSpecs.Title == "")
        m_AppConfig.Input_WindowSpecs.Title = GetAppInfo().Name;
    m_Window = Platform::CreateWindow(m_AppConfig.Input_WindowSpecs);
    m_Window->SetEventCallback(
        std::bind(&Application::ConsumeEvent, this, std::placeholders::_1));
    m_Window->Show();

    // Create Graphics Context
    m_Context = Platform::CreateGraphicsContext(*m_Window);
    m_Gapi = m_Context->GetAPI();
    m_Gapi->SetClearColor(m_AppConfig.Rendering_ClearColor);

    // Graphics Pipeline
    m_Graphics = CreateScope<GraphicsPipeline>(*m_Context);
    UI::Initialize(m_Window);

    // Main Logic
    if (m_AppConfig.Input_ScanForControllersOnLaunch)
        m_Window->ScanForControllers();
    OnStartup();

    while (m_Run) {
        // Propergate Events
        m_Assets->EnsureDestructors();
        Time::CountFrame();
        Input::NewFrame(); // Resets all pressed bits
        m_Window->PollEvents();
        HandleEvents();

        // Update world
        for (Layer *layer : m_Layerstack)
            layer->Update(Time::Delta());
        m_World.progress(Time::Delta().count() * 0.001f);

        m_Graphics->RenderFrame();
    }

    OnClose();
}

void Application::Stop() {
    MC_INFO("Application Stop Requested");
    m_Run = false;
}

void Application::HandleEvents() {
    while (!m_EventsQueue.empty()) {
        const auto e = m_EventsQueue.front();
        EventDispatcher dispatcher(e);
        m_EventsQueue.pop();

        dispatcher.Dispatch<WindowCloseEvent>(
            M_BIND_EVENT_FN(Application::OnWindowClose));
        dispatcher.Dispatch<WindowResizeEvent>(
            M_BIND_EVENT_FN(Application::OnWindowResize));
        dispatcher.Dispatch<WindowFramebufferResizeEvent>(
            M_BIND_EVENT_FN(Application::OnFramebufferResize));
        Input::Event(*e); // Stores input state
        UI::OnEvent(e);
        m_Graphics->OnEvent(e);

        // Send the event down the layerstack (from top to bottom)
        for (auto it = m_Layerstack.end(); it != m_Layerstack.begin();) {
            if (e->Handled)
                break;
            (*--it)->OnEvent(*e);
        }

        if (!e->Handled) {
            // Since it's not handled by the layers, send it out into the world
            EventBridge::CallObseverComponents(e);
        }

        delete e;
    }
}

bool Application::OnWindowFocus(const WindowFocusEvent &e) { return false; }
bool Application::OnWindowResize(const WindowResizeEvent &e) {
    MC_INFO("Window logical size change to {}x{}", e.GetWidth(), e.GetHeight());

    return false;
}

bool Application::OnFramebufferResize(const WindowFramebufferResizeEvent &e) {
    MC_INFO("Window framebuffer size change to {}x{}", e.GetWidth(),
            e.GetHeight());

    return false;
}

bool Application::OnWindowClose(const WindowCloseEvent &e) {
    MC_INFO("Window Closed!");
    Stop();

    return true;
}

void Application::ConsumeEvent(Event *event) { m_EventsQueue.push(event); }
} // namespace Mana
