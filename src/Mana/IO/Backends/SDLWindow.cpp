#include "Mana/Events/IOEvents.h"
#include "Mana/IO/KeyCodes.h"
#include "Mana/IO/Window.h"

#include "SDLWindow.h"

namespace Mana {
static std::unordered_map<SDL_Keycode, Key::KeyCode> SDLKeycodeMap = {
    {SDL_SCANCODE_SPACE, Key::SPACE},
    {SDL_SCANCODE_APOSTROPHE, Key::APOSTROPHE},
    {SDL_SCANCODE_COMMA, Key::COMMA},
    {SDL_SCANCODE_MINUS, Key::MINUS},
    {SDL_SCANCODE_PERIOD, Key::PERIOD},
    {SDL_SCANCODE_SLASH, Key::SLASH},
    {SDL_SCANCODE_0, Key::N0},
    {SDL_SCANCODE_1, Key::N1},
    {SDL_SCANCODE_2, Key::N2},
    {SDL_SCANCODE_3, Key::N3},
    {SDL_SCANCODE_4, Key::N4},
    {SDL_SCANCODE_5, Key::N5},
    {SDL_SCANCODE_6, Key::N6},
    {SDL_SCANCODE_7, Key::N7},
    {SDL_SCANCODE_8, Key::N8},
    {SDL_SCANCODE_9, Key::N9},
    {SDL_SCANCODE_SEMICOLON, Key::SEMICOLON},
    {SDL_SCANCODE_A, Key::A},
    {SDL_SCANCODE_B, Key::B},
    {SDL_SCANCODE_C, Key::C},
    {SDL_SCANCODE_D, Key::D},
    {SDL_SCANCODE_E, Key::E},
    {SDL_SCANCODE_F, Key::F},
    {SDL_SCANCODE_G, Key::G},
    {SDL_SCANCODE_H, Key::H},
    {SDL_SCANCODE_I, Key::I},
    {SDL_SCANCODE_J, Key::J},
    {SDL_SCANCODE_K, Key::K},
    {SDL_SCANCODE_L, Key::L},
    {SDL_SCANCODE_M, Key::M},
    {SDL_SCANCODE_N, Key::N},
    {SDL_SCANCODE_O, Key::O},
    {SDL_SCANCODE_P, Key::P},
    {SDL_SCANCODE_Q, Key::Q},
    {SDL_SCANCODE_R, Key::R},
    {SDL_SCANCODE_S, Key::S},
    {SDL_SCANCODE_T, Key::T},
    {SDL_SCANCODE_U, Key::U},
    {SDL_SCANCODE_V, Key::V},
    {SDL_SCANCODE_W, Key::W},
    {SDL_SCANCODE_X, Key::X},
    {SDL_SCANCODE_Y, Key::Y},
    {SDL_SCANCODE_Z, Key::Z},
    {SDL_SCANCODE_LEFTBRACKET, Key::LEFT_BRACKET},
    {SDL_SCANCODE_BACKSLASH, Key::BACKSLASH},
    {SDL_SCANCODE_GRAVE, Key::GRAVE_ACCENT},
    {SDL_SCANCODE_RIGHTBRACKET, Key::RIGHT_BRACKET},
    {SDL_SCANCODE_Z, Key::Z},
    {SDL_SCANCODE_F4, Key::F4},

    // Function keys TODO: FUNCTION KEYS, KP. Essentialy from 256 and above
};
static int k(SDL_Keycode key) {
    auto it = SDLKeycodeMap.find(key);
    if (it != SDLKeycodeMap.end())
        return it->second;
    MC_WARN("Unkown keycode {}", key);
    return 0; // fallback
}

SDLWindow::SDLWindow(WindowSpecs &specs) {
    // Ensure that SDL_VIDEO is initialized
    MC_INFO("Intializing SDL_VIDEO (SDL v{}.{}.{})",
            SDL_VERSIONNUM_MAJOR(SDL_VERSION),
            SDL_VERSIONNUM_MINOR(SDL_VERSION),
            SDL_VERSIONNUM_MICRO(SDL_VERSION));

    if (!(SDL_WasInit(SDL_INIT_VIDEO) & SDL_INIT_VIDEO)) {
        if (!SDL_InitSubSystem(SDL_INIT_VIDEO)) {
            MC_CRITICAL("Failed to initialize SDL Video");
            MC_CRITICAL("SDLError: {}", SDL_GetError());
            return;
        }
    }

// By default OpenGL 1.1 is only loaded, lets change that
#ifdef MANAB_OPENGL
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS,
                        SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);
#ifdef MANAO_OSX
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
#else
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
#endif
#endif

    // Create window
    m_Fullscreen = specs.Fullscreen;
    m_NativePtr = SDL_CreateWindow(
        specs.Title.c_str(), specs.Width, specs.Height,
        SDL_WINDOW_OPENGL | (specs.Hidden ? SDL_WINDOW_HIDDEN : 0) |
            (specs.Resizeable ? SDL_WINDOW_RESIZABLE : 0) |
            (specs.Fullscreen ? SDL_WINDOW_FULLSCREEN : 0));
    if (m_NativePtr == NULL) {
        MC_CRITICAL("Failed to create SDL Window");
        MC_CRITICAL("SDLError: {}", SDL_GetError());
        return;
    }

    if (!m_Fullscreen && specs.Position == WindowSpecs::Centered)
        CenterWindow();

    // On a high density screen this value might be different than what we gave
    // in CreateWindow
    SDL_GetWindowSizeInPixels(m_NativePtr, &m_FramebufferSize.x,
                              &m_FramebufferSize.y);
}
SDLWindow::~SDLWindow() {}

#ifdef MANAB_OPENGL
void (*(*SDLWindow::CreateGLContext())(const char *))(void) {
    SDL_GL_CreateContext(m_NativePtr);
    return SDL_GL_GetProcAddress;
}

void SDLWindow::GLSwapBuffers() { SDL_GL_SwapWindow(m_NativePtr); }
#endif

void SDLWindow::PollEvents() {
    SDL_Event event = {0};
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_EVENT_QUIT:
            m_EventCallback(new WindowCloseEvent);
            break;
        case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
            m_FramebufferSize = {event.window.data1, event.window.data2};
            m_EventCallback(new WindowFramebufferResizeEvent(
                event.window.data1, event.window.data2));
            break;
        case SDL_EVENT_WINDOW_RESIZED:
            m_WindowSize = {event.window.data1, event.window.data2};
            m_EventCallback(
                new WindowResizeEvent(event.window.data1, event.window.data2));
            break;
        case SDL_EVENT_WINDOW_FOCUS_GAINED:
            m_EventCallback(new WindowFocusEvent);
            break;
        case SDL_EVENT_WINDOW_FOCUS_LOST:
            m_EventCallback(new WindowLostFocusEvent);
            break;
        case SDL_EVENT_WINDOW_MOVED:
            m_EventCallback(
                new WindowMovedEvent(event.window.data1, event.window.data2));
            break;
        case SDL_EVENT_KEY_DOWN:
            m_EventCallback(
                new KeyPressedEvent(k(event.key.scancode), event.key.repeat));
            break;
        case SDL_EVENT_KEY_UP:
            m_EventCallback(new KeyReleasedEvent(k(event.key.scancode)));
            break;
        case SDL_EVENT_MOUSE_MOTION:
            m_EventCallback(new MouseMovedEvent(event.motion.x, event.motion.y,
                                                event.motion.xrel,
                                                event.motion.yrel));
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            m_EventCallback(new ButtonPressedEvent(
                event.button.button -
                1)); // The -1, converts the button to our button codes. Simple
            break;
        case SDL_EVENT_MOUSE_BUTTON_UP:
            m_EventCallback(new ButtonReleasedEvent(event.button.button - 1));
            break;
        case SDL_EVENT_MOUSE_WHEEL:
            m_EventCallback(
                new MouseScrolledEvent(event.wheel.x, event.wheel.y));
            break;
            // TODO: JOYSTICK EVENTS
        }
    }
}
void SDLWindow::Show() { SDL_ShowWindow(m_NativePtr); }
void SDLWindow::Fullscreen(bool fullscreen, const Monitor &monitor) {
    MC_WARN("NOT YET IMPLEMENTED!");
    return;
}
void SDLWindow::SetTitle(std::string &title) {
    SDL_SetWindowTitle(m_NativePtr, title.c_str());
}
void SDLWindow::SetPosition(int x, int y) {
    if (!SDL_SetWindowPosition(m_NativePtr, x, y)) {
        MC_ERROR("Failed to set window position: {}", SDL_GetError());
    }
}
void SDLWindow::CenterWindow() {
    SDL_SetWindowPosition(m_NativePtr, SDL_WINDOWPOS_CENTERED,
                          SDL_WINDOWPOS_CENTERED);
    return;
}
void SDLWindow::ScanForControllers() {
    MC_WARN("NOT YET IMPLEMENTED!");
    MC_WARN("JoyStick/Gamepad Support is not yet available on the SDL window "
            "backend.");
    MC_WARN(
        "Please switch to the GLFW platform if you wish to use controllers.");
    return;
}

// Window State Get
[[nodiscard]] bool SDLWindow::ShouldWindowClose() const {
    MC_WARN("Not implemented on SDL");
    return false;
}
[[nodiscard]] bool SDLWindow::IsFullscreen() const { return m_Fullscreen; }
} // namespace Mana
