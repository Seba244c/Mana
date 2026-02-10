#pragma once
#include "mpch.h"

#include "Mana/Graphics/Framebuffer.h"
#include "Mana/Graphics/GraphicsContext.h"
#include "Mana/Graphics/Shader.h"
#include "Mana/Rendering/Pipeline.h"
#include "Mana/Rendering/Renderer.h"

namespace Mana {
class PPOutline : public RenderPass {
  public:
    PPOutline(GraphicsContext &graphics);
    void OnWindowResize(const WindowResizeEvent &e) override;
    void OnFramebufferResize(const WindowFramebufferResizeEvent &e) override;
    void Execute(FrameData *data) override;

  private:
    Renderer2D m_Renderer;

    Ref<Shader> m_OutlineShader;
    Ref<Framebuffer> m_AlphaMaskFBO;
    Ref<Shader> m_AlphaMaskShader;

    Ref<VertexArray> m_VAO;
    Ref<VertexBuffer> m_VBO;

    float m_TexelW, m_TexelH;
};
} // namespace Mana
