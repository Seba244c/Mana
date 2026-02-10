#include "GLFWWindow.h"
#include "Mana/Core/Application.h"
#include "Mana/Core/Log.h"
#include "Mana/Events/IOEvents.h"
#include "Mana/IO/Window.h"

namespace Mana {
class GLFWController final : public Controller {
  public:
    std::string_view Name() const override { return m_Name; }

    void PollState() override { glfwGetGamepadState(m_Id, &m_State); }

    bool Button(ControllerButton b) const override {
        return m_State.buttons[b] == GLFW_PRESS;
    }

    float Axis(ControllerAxis b) const override { return m_State.axes[b]; }

  protected:
    std::string m_Name;
    ControllerBackendID m_Id;
    GLFWgamepadstate m_State;

    friend Scope<Controller> GetControllerInfo(ControllerID cId);
};

static void ErrorCallback(int code, const char *description) {
    MC_CRITICAL("GLFW Error (Code: {}): {}", code, description);
}

Monitor GLFWMonitor(GLFWmonitor *monitor) {
    Monitor out;
    out.Name = glfwGetMonitorName(monitor);
    out.Primary = glfwGetPrimaryMonitor() == monitor;
    out.NativeHandle = monitor;

    const GLFWvidmode *mode = glfwGetVideoMode(monitor);
    if (mode) {
        out.Width = mode->width;
        out.Height = mode->height;
        out.RefreshRate = mode->refreshRate;
    }

    return out;
}

int GLFWWindow::s_GLFWWindows = 0;

bool GLFWWindow::InitializeGLFW() {
    MC_TRACE("Initializing GLFW");
    glfwSetErrorCallback(ErrorCallback);

    glfwInitHint(GLFW_WAYLAND_LIBDECOR,
                 GLFW_WAYLAND_DISABLE_LIBDECOR); // Fix for libdecor error on
                                                 // GLFW with hyprland

    if (!glfwInit()) {
        MC_ERROR("GLFW Failed to initialize GLFW!");

        return false;
    }

    return true;
}

void GLFWWindow::TerminateGLFW() {
    MC_TRACE("Terminating GLFW");
    glfwTerminate();
}

Scope<Controller> GetControllerInfo(ControllerID cId) {
    Scope<GLFWController> c = CreateScope<GLFWController>();
    c->m_Id = cId;

    if (auto name = glfwGetGamepadName(cId)) {
        MC_TRACE("Found gamepad mappings!");
        c->m_Name = name;
    } else {
        MC_WARN("Gamepad mappings was not found. Using raw joystick info.");
        c->m_Name = glfwGetJoystickName(cId);
    }

    return std::move(c);
}

void GLFWWindow::ScanForControllers() {
    MC_TRACE("Scanning for joysticks.");
    for (int i = GLFW_JOYSTICK_1; i <= GLFW_JOYSTICK_LAST; i++) {
        if (glfwJoystickPresent(i)) {
            MC_TRACE("Joystick {} present", i);
            if (Application::Instance()
                    ->GetConfig()
                    .Input_GamepadMappedControllersOnly &&
                !glfwGetGamepadName(i)) {
                MC_WARN("Ignoring joystick {} ({}), because it is not a "
                        "gamepad!",
                        glfwGetJoystickName(i), i);
                continue;
            }

            Application::Instance()->ConsumeEvent(new ControllerConnectedEvent(
                i, std::move(GetControllerInfo(i))));
        }
    }
}

GLFWWindow::GLFWWindow(WindowSpecs &specs) {
    if (s_GLFWWindows++ < 1 && !InitializeGLFW())
        return;

// Window creation hints
#ifdef MANAB_OPENGL
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

#ifdef MANAO_OSX
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
#endif

#else
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#endif

    MC_INFO("GLFWWindowHint Resiable {}, Hidden {}", specs.Resizeable,
            specs.Hidden);
    glfwWindowHint(GLFW_RESIZABLE, specs.Resizeable ? GLFW_TRUE : GLFW_FALSE);

    // The window must be shown to fullscreen to work
    glfwWindowHint(GLFW_VISIBLE, specs.Fullscreen
                                     ? true
                                     : (specs.Hidden ? GLFW_FALSE : GLFW_TRUE));

    if (!specs.Fullscreen && specs.Position == WindowSpecs::Centered) {
        const GLFWvidmode *vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        int x = (vidmode->width - specs.Width) * 0.5f;
        int y = (vidmode->height - specs.Height) * 0.5f;
        glfwWindowHint(GLFW_POSITION_X, x);
        glfwWindowHint(GLFW_POSITION_Y, y);
    }

    // Create Window
    MC_INFO("Creating GLFWWindow (Title: {}, Size: {}x{})", specs.Title,
            specs.Width, specs.Height);
    m_NativePtr = glfwCreateWindow(specs.Width, specs.Height,
                                   specs.Title.c_str(), NULL, NULL);

    if (!m_NativePtr) {
        const char *description = nullptr;
        glfwGetError(&description);
        MC_ERROR("IO:WIN:GLFW Failed to create GLFW window: {}", description);

        if (--s_GLFWWindows < 1)
            TerminateGLFW();
    }

    // Save State
    glfwSetWindowUserPointer(m_NativePtr, &m_State);

    int fw, fh;
    glfwGetFramebufferSize(m_NativePtr, &fw, &fh);
    m_State.WidthWindow = specs.Width;
    m_State.HeightWindow = specs.Height;
    m_State.WidthFramebuffer = fw;
    m_State.HeightFrameBuffer = fh;

    // Create Callbacks
    glfwSetFramebufferSizeCallback(m_NativePtr, [](GLFWwindow *window,
                                                   int width, int height) {
        WindowState &state =
            *static_cast<WindowState *>(glfwGetWindowUserPointer(window));
        state.WidthFramebuffer = width;
        state.HeightFrameBuffer = height;

        if (state.EventCallback) // This might be null, in case of fullscreening
                                 // the window before this is set
            state.EventCallback(
                new WindowFramebufferResizeEvent(width, height));
    });

    glfwSetWindowSizeCallback(m_NativePtr, [](GLFWwindow *window, int width,
                                              int height) {
        WindowState &state =
            *static_cast<WindowState *>(glfwGetWindowUserPointer(window));
        state.WidthWindow = width;
        state.HeightWindow = height;

        if (state.EventCallback) // This might be null, in case of fullscreening
                                 // the window before this is set
            state.EventCallback(new WindowResizeEvent(width, height));
    });

    glfwSetWindowCloseCallback(m_NativePtr, [](GLFWwindow *window) {
        const WindowState &state =
            *static_cast<WindowState *>(glfwGetWindowUserPointer(window));
        state.EventCallback(new WindowCloseEvent);
    });

    glfwSetWindowFocusCallback(
        m_NativePtr, [](GLFWwindow *window, int focused) {
            const WindowState &state =
                *static_cast<WindowState *>(glfwGetWindowUserPointer(window));
            state.EventCallback(focused ? (Event *)new WindowFocusEvent
                                        : (Event *)new WindowLostFocusEvent);
        });

    glfwSetWindowPosCallback(
        m_NativePtr, [](GLFWwindow *window, int xpos, int ypos) {
            const WindowState &state =
                *static_cast<WindowState *>(glfwGetWindowUserPointer(window));
            state.EventCallback(new WindowMovedEvent(xpos, ypos));
        });

    glfwSetKeyCallback(m_NativePtr, [](GLFWwindow *window, int key,
                                       int scancode, int action, int mods) {
        const WindowState &state =
            *static_cast<WindowState *>(glfwGetWindowUserPointer(window));

        switch (action) {
        case GLFW_REPEAT: {
            state.EventCallback(new KeyPressedEvent(key, true));
            break;
        }
        case GLFW_PRESS: {
            state.EventCallback(new KeyPressedEvent(key, false));
            break;
        }
        case GLFW_RELEASE: {
            state.EventCallback(new KeyReleasedEvent(key));
            break;
        }
        default:
            MC_WARN("Unkown key action: {}", action);
        }
    });

    glfwSetCursorPosCallback(
        m_NativePtr, [](GLFWwindow *window, double xPos, double yPos) {
            WindowState &state =
                *static_cast<WindowState *>(glfwGetWindowUserPointer(window));
            const float dx = static_cast<float>(xPos) - state.MouseX;
            const float dy = static_cast<float>(yPos) - state.MouseY;
            state.MouseX = static_cast<float>(xPos);
            state.MouseY = static_cast<float>(yPos);
            state.EventCallback(
                new MouseMovedEvent(state.MouseX, state.MouseY, dx, dy));
        });

    glfwSetMouseButtonCallback(
        m_NativePtr, [](GLFWwindow *window, int button, int action, int mods) {
            const WindowState &state =
                *static_cast<WindowState *>(glfwGetWindowUserPointer(window));

            switch (action) {
            case GLFW_PRESS: {
                state.EventCallback(new ButtonPressedEvent(button));
                break;
            }
            case GLFW_RELEASE: {
                state.EventCallback(new ButtonReleasedEvent(button));
                break;
            }
            default:
                MC_WARN("Unkown mouse action: {}", action);
            }
        });

    glfwSetScrollCallback(
        m_NativePtr, [](GLFWwindow *window, double xOffset, double yOffset) {
            const WindowState &state =
                *static_cast<WindowState *>(glfwGetWindowUserPointer(window));
            state.EventCallback(new MouseScrolledEvent(
                static_cast<float>(xOffset), static_cast<float>(yOffset)));
        });

    glfwSetJoystickCallback([](int joystick, int event) {
        MC_TRACE("GLFWJoyStickCallback");
        if (event == GLFW_CONNECTED) {
            if (Application::Instance()
                    ->GetConfig()
                    .Input_GamepadMappedControllersOnly &&
                !glfwGetGamepadName(joystick)) {
                MC_WARN("Ignoring joystick {} ({}), because it is not a "
                        "gamepad!",
                        glfwGetJoystickName(joystick), joystick);
                return;
            }

            Application::Instance()->ConsumeEvent(new ControllerConnectedEvent(
                joystick, GetControllerInfo(joystick)));
        } else if (event == GLFW_DISCONNECTED) {
            Application::Instance()->ConsumeEvent(
                new ControllerDisconnectedEvent(joystick));
        } else {
            MC_WARN("GLFWWindow Recieved Unsupported Joystick Event");
        }
    });

    // Get monitors
    int count;
    GLFWmonitor **glfwMonitors = glfwGetMonitors(&count);
    for (int i = 0; i < count; i++) {
        m_Monitors.push_back(GLFWMonitor(glfwMonitors[i]));
    }

    // Make fullscreen if required by specs
    if (specs.Fullscreen)
        Fullscreen(true, Window::PickBestMonitor(m_Monitors));
}

GLFWWindow::~GLFWWindow() {
    MC_TRACE("Destroying GLFWWindow");

    if (--s_GLFWWindows < 1)
        TerminateGLFW();
}

#ifdef MANAB_OPENGL
void (*(*GLFWWindow::CreateGLContext())(const char *))(void) {
    glfwMakeContextCurrent(m_NativePtr);
    return glfwGetProcAddress;
}

void GLFWWindow::GLSwapBuffers() { glfwSwapBuffers(m_NativePtr); }
#endif

bool GLFWWindow::ShouldWindowClose() const {
    return glfwWindowShouldClose(m_NativePtr);
}

void GLFWWindow::SetTitle(std::string &title) {
    MC_INFO("Setting title to: {}", title);
    glfwSetWindowTitle(m_NativePtr, title.c_str());
}

void GLFWWindow::SetPosition(int x, int y) {
    MC_INFO("Setting window position to: {},{}", x, y);
    glfwSetWindowPos(m_NativePtr, x, y);
}

void GLFWWindow::CenterWindow() {
    MC_INFO("Centering window");
    const GLFWvidmode *vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    int w, h;
    glfwGetWindowSize(m_NativePtr, &w, &h);
    int x = (vidmode->width - w) * 0.5f;
    int y = (vidmode->height - h) * 0.5f;
    SetPosition(x, y);
}

void GLFWWindow::PollEvents() {
    glfwPollEvents();

    // Update gamepad states
}

void GLFWWindow::Show() { glfwShowWindow(m_NativePtr); }
void GLFWWindow::Fullscreen(bool fullscreen, const Monitor &monitorSpec) {
    GLFWmonitor *monitor =
        reinterpret_cast<GLFWmonitor *>(monitorSpec.NativeHandle);
    if (!monitor) {
        MC_WARN("Invalid monitor given to Fullscreen, selecting primary.");
        monitor = glfwGetPrimaryMonitor();

        if (monitor == nullptr)
            return;
    }

    MC_TRACE("GLFWWindow::Fullscreen(fullscreen = {}, monitor={})", fullscreen,
             monitorSpec.Name);

    m_Fullscreen = fullscreen;

    if (fullscreen) {
        Show(); // The window must be shown for fullscreening to work
        const GLFWvidmode *mode = glfwGetVideoMode(monitor);

        if (glfwGetPlatform() !=
            GLFW_PLATFORM_WAYLAND) { // Wayland does not provide this
            glfwGetWindowPos(m_NativePtr, &m_storedX, &m_storedY);
            glfwGetWindowSize(m_NativePtr, &m_storedW, &m_storedH);
        }

        glfwSetWindowMonitor(
            m_NativePtr,
            monitor, // Providing monitor tells GLFW to make it fullscreen
            0, 0,    // Theese position arguments are ignored
            mode->width, mode->height, mode->refreshRate);
        MC_INFO("GLFWWindow changed to fullscreen ({}x{}@{} on {})",
                mode->width, mode->height, mode->refreshRate,
                glfwGetMonitorName(monitor));
    } else {
        glfwSetWindowMonitor(
            m_NativePtr,
            nullptr, // nullptr is what tells GLFW to make it windowed
            // The position will be if runniong ignored on Wayland
            m_storedX, m_storedY, m_storedW, m_storedH,
            0 // The refresh-rate argument is ignored
        );
        MC_INFO("GLFWWindow changed to windowed");
    }
}

bool GLFWWindow::IsFullscreen() const { return false; }
} // namespace Mana
