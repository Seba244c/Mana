#pragma once
#include "mpch.h"

#include "Mana/IO/Window.h"

namespace Mana {
class GLFWWindow final : public Window {
  public:
    GLFWWindow(WindowSpecs &specs);
    ~GLFWWindow();

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
    glm::ivec2 GetWindowSize() override {
        return {m_State.WidthWindow, m_State.HeightWindow};
    }

    glm::ivec2 GetFramebufferSize() override {
        return {m_State.WidthFramebuffer, m_State.HeightFrameBuffer};
    }
    void SetEventCallback(const EventCallbackFn &callback) override {
        m_State.EventCallback = callback;
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
    static int s_GLFWWindows;
    static bool InitializeGLFW();
    static void TerminateGLFW();
    GLFWwindow *m_NativePtr;
    std::vector<Monitor> m_Monitors;

    struct WindowState {
        EventCallbackFn EventCallback;
        unsigned int WidthWindow, HeightWindow, WidthFramebuffer,
            HeightFrameBuffer;

        float MouseX, MouseY;
    };
    WindowState m_State;

    bool m_Fullscreen = false;
    int m_storedX = 0, m_storedY = 0, m_storedW = 0,
        m_storedH = 0; // Used to save window state before entering fullscreen
};
} // namespace Mana
