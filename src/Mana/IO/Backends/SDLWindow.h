#pragma once
#include "mpch.h"

#include "Mana/IO/Window.h"

namespace Mana {
class SDLWindow final : public Window {
  public:
    SDLWindow(WindowSpecs &specs);
    ~SDLWindow();

#ifdef MANAB_OPENGL
    void (*(*CreateGLContext())(const char *))(void) override;
    void GLSwapBuffers() override;
#endif

    // Window State Set
    void PollEvents() override;
    void Show() override;
    void Fullscreen(bool fullscreen, const Monitor &monitor) override;
    void SetTitle(std::string &title) override;
    void SetPosition(int x, int y) override;
    void CenterWindow() override;
    void ScanForControllers() override;
    glm::ivec2 GetWindowSize() override { return m_WindowSize; }
    glm::ivec2 GetFramebufferSize() override { return m_FramebufferSize; }
    void SetEventCallback(const EventCallbackFn &callback) override {
        m_EventCallback = callback;
    }

    // Window State Get
    [[nodiscard]] bool ShouldWindowClose() const override;
    [[nodiscard]] bool IsFullscreen() const override;
    [[nodiscard]] virtual void *GetNativeWindow() const override {
        return m_NativePtr;
    };
    [[nodiscard]] const std::vector<Monitor> &GetMonitors() const override {
        return m_Monitors;
    }

  private:
    std::vector<Monitor> m_Monitors;
    SDL_Window *m_NativePtr;
    EventCallbackFn m_EventCallback;
    glm::ivec2 m_WindowSize, m_FramebufferSize;
    bool m_Fullscreen;
};
} // namespace Mana
