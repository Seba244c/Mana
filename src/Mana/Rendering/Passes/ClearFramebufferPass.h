#pragma once
#include <mpch.h>

#include "Mana/Rendering/Pipeline.h"

namespace Mana {
class ClearFramebufferPass : public RenderPass {
  public:
    ClearFramebufferPass(GraphicsContext &context, Color clearColor)
        : RenderPass(context), m_Color(clearColor) {}
    void OnWindowResize(const WindowResizeEvent &e) override {}
    void OnFramebufferResize(const WindowFramebufferResizeEvent &e) override {}
    void Execute(FrameData *frame) override;

  private:
    Color m_Color;
};
} // namespace Mana
