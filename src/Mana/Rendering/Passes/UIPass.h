#pragma once
#include "mpch.h"

#include "Mana/Rendering/Pipeline.h"
#include "Mana/Rendering/Renderer.h"

namespace Mana {
class UIPass : public RenderPass {
  public:
    UIPass(GraphicsContext &context);
    void OnWindowResize(const WindowResizeEvent &e) override;
    void OnFramebufferResize(const WindowFramebufferResizeEvent &e) override;
    void Execute(FrameData *frameData) override;

  private:
    Renderer2D m_Renderer;
};
} // namespace Mana
