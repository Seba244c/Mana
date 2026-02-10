#pragma once
#include <mpch.h>

#include "Mana/Audio/Audio.h"
#include "Mana/Core/Asset.h"
#include "Mana/Core/Layer.h"
#include "Mana/Events/Event.h"
#include "Mana/Events/IOEvents.h"
#include "Mana/Graphics/GraphicsAPI.h"
#include "Mana/Graphics/GraphicsContext.h"
#include "Mana/IO/Window.h"
#include "Mana/Rendering/Pipeline.h"
#include "Mana/Services/Service.h"

namespace Mana {
struct AppConfig {
    bool HotCompileAssets = true;
    bool Assets_GarbageCollection = true;
    std::vector<std::filesystem::path> Assets_SourceFolders;

    Color Rendering_ClearColor = Colors::Azure;

    bool Input_ScanForControllersOnLaunch =
        true; // Systems like GLFW only inform of controllers when they connect
              // or disconnect by default, by scanning at launch, we can use
              // controllers who are already connected at launch
    bool Input_GamepadMappedControllersOnly = true;
    WindowSpecs Input_WindowSpecs;
};

struct AppInfo {
    std::string Name;
    std::string Author;
    std::string Version;
    std::string Build;
};

class Application {
  public:
    virtual ~Application() = default;
    const virtual AppInfo GetAppInfo() = 0;

    // Event is owned by application when it is submitted via this function
    // Assume the pointer to be deleted afterwards
    void ConsumeEvent(Event *event);

    void Start();
    void Stop();

    // Getters
    [[nodiscard]] GraphicsAPI &GetGraphicsAPI() { return *m_Gapi; }
    [[nodiscard]] AppConfig &GetConfig() { return m_AppConfig; }
    [[nodiscard]] Window &GetWindow() { return *m_Window; }
    [[nodiscard]] AssetManager &GetAssets() { return *m_Assets; }
    [[nodiscard]] ServicesManager &GetServices() { return *m_Services; }
    [[nodiscard]] AudioManager &GetAudio() { return *m_Audio; }
    [[nodiscard]] Layerstack &GetLayerstack() { return m_Layerstack; }
    [[nodiscard]] GraphicsPipeline &GetGraphicsPipeline() {
        return *m_Graphics;
    }
    [[nodiscard]] flecs::world &World() { return m_World; }

  protected:
    virtual void OnStartup() = 0;
    virtual void OnClose() = 0;

  protected:
    AppConfig m_AppConfig;
    Scope<Window> m_Window;
    Scope<GraphicsContext> m_Context;
    Scope<AssetManager> m_Assets;
    Scope<GraphicsAPI> m_Gapi;
    Scope<ServicesManager> m_Services;
    Scope<AudioManager> m_Audio;
    Scope<GraphicsPipeline> m_Graphics;
    flecs::world m_World;
    Layerstack m_Layerstack;

  private:
    void HandleEvents();
    bool OnWindowClose(const WindowCloseEvent &e);
    bool OnWindowFocus(const WindowFocusEvent &e);
    bool OnWindowResize(const WindowResizeEvent &e);
    bool OnFramebufferResize(const WindowFramebufferResizeEvent &e);

  private:
    bool m_Run = true;
    std::queue<Event *> m_EventsQueue;

  public:
    static Application *Instance() { return m_Instance; }

  private:
    static Application *m_Instance;
};

int StartApplication(Application *app);
} // namespace Mana
