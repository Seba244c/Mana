#pragma once
#include <mpch.h>

#include "Mana/Events/Event.h"

namespace Mana {
struct WindowSpecs {
    enum WindowCreationPos {
        Default = 0,
        Centered,
    };

    std::string Title;
    int Width = 1280;
    int Height = 720;
    bool Fullscreen = false;
    bool Resizeable = true;
    bool Hidden = true;
    WindowCreationPos Position = WindowSpecs::Centered;
};

struct Monitor {
    bool Primary;
    uint32_t Width;
    uint32_t Height;
    float RefreshRate;
    std::string Name;
    void *NativeHandle;

    Monitor()
        : Primary(false), Width(0), Height(0), RefreshRate(0),
          NativeHandle(nullptr) {}
};

class Window {
  public:
    virtual ~Window() = default;
#ifdef MANAB_OPENGL
    virtual void (*(*CreateGLContext())(const char *))(void) = 0;
    virtual void GLSwapBuffers() = 0;
#endif

    // Window State Set
    virtual void PollEvents() = 0;
    virtual void Show() = 0;
    virtual void Fullscreen(bool fullscreen, const Monitor &monitor) = 0;
    virtual void SetTitle(std::string &title) = 0;
    virtual void SetPosition(int x, int y) = 0;
    virtual void CenterWindow() = 0;
    virtual void ScanForControllers() = 0;
    virtual glm::ivec2 GetWindowSize() = 0;
    virtual glm::ivec2 GetFramebufferSize() = 0;

    using EventCallbackFn = std::function<void(Event *)>;
    virtual void SetEventCallback(const EventCallbackFn &callback) = 0;

    // Window State Get
    [[nodiscard]] virtual bool ShouldWindowClose() const = 0;
    [[nodiscard]] virtual bool IsFullscreen() const = 0;
    [[nodiscard]] virtual void *GetNativeWindow() const = 0;
    [[nodiscard]] virtual const std::vector<Monitor> &GetMonitors() const = 0;

  public:
    static const Monitor &PickBestMonitor(const std::vector<Monitor> &monitors);
};
} // namespace Mana
