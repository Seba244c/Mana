#pragma once
#include "mpch.h"

#include "Mana/Rendering/Pipeline.h"
#include "Mana/Rendering/Renderer.h"

namespace Mana {
class ScenePass : public RenderPass {
  public:
    ScenePass(GraphicsContext &context);
    void OnWindowResize(const WindowResizeEvent &e) override;
    void OnFramebufferResize(const WindowFramebufferResizeEvent &e) override;
    void Execute(FrameData *) override;

  private:
    void DrawTilemaps(FrameData *);
    void DrawSprites(FrameData *);

    Renderer2D m_Renderer;
};
} // namespace Mana
