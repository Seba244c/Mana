#pragma once
#include "mpch.h"

#include "Mana/Graphics/GraphicsAPI.h"
#include "Mana/Graphics/GraphicsContext.h"
#include "Mana/IO/Window.h"

namespace Mana {
struct GLInfo {
    int32_t MaxFragmentSamplers = 16;
};

class GLContext : public GraphicsContext {
  public:
    GLContext(Window &window);
    ~GLContext();
    Scope<GraphicsAPI> GetAPI() override;

  private:
    void SwapWindowDrawBuffers();
    Window *m_Window;

    GLInfo m_GLInfo;
};
} // namespace Mana
