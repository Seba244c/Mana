#include "PPOutline.h"

#include "Mana/Core/Application.h"

namespace Mana {
PPOutline::PPOutline(GraphicsContext &graphics)
    : RenderPass(graphics), m_Renderer(graphics) {
    // Create the target framebuffer
    glm::ivec2 fboSize =
        Application::Instance()->GetWindow().GetFramebufferSize();
    m_AlphaMaskFBO = API->CreateFBO(
        {static_cast<uint32_t>(fboSize.x), static_cast<uint32_t>(fboSize.y),
         FramebufferAttachmentSpecification{{FramebufferTextureFormat::R8}}});
    m_TexelW = 1.0f / m_AlphaMaskFBO->GetSpecification().Width;
    m_TexelH = 1.0f / m_AlphaMaskFBO->GetSpecification().Height;

    // Load resources
    m_AlphaMaskShader = M_ASSET(Shader, "shaders/PP_OutlineAlphaMask.glsl");
    m_OutlineShader = M_ASSET(Shader, "shaders/PP_Outline.glsl");
    m_Renderer.SetQuadShader(m_AlphaMaskShader);

    // VAO + VBO
    float quadVertices[] = {// positions   // texCoords
                            -1.0f, 1.0f, 0.0f, 1.0f,  -1.0f, -1.0f,
                            0.0f,  0.0f, 1.0f, -1.0f, 1.0f,  0.0f,

                            -1.0f, 1.0f, 0.0f, 1.0f,  1.0f,  -1.0f,
                            1.0f,  0.0f, 1.0f, 1.0f,  1.0f,  1.0f};
    m_VAO = API->CreateVertexArray();
    m_VBO = API->CreateVertexBuffer(sizeof(quadVertices));
    m_VBO->SetLayout({{ShaderDataType::Float2, "a_Position"},
                      {ShaderDataType::Float2, "a_TexCoord"}});
    m_VBO->SetData(quadVertices, sizeof(quadVertices));
    m_VAO->AddVertexBuffer(m_VBO);
}
void PPOutline::OnFramebufferResize(const WindowFramebufferResizeEvent &e) {
    m_AlphaMaskFBO->Resize(e.GetWidth(), e.GetHeight());
    m_TexelW = 1.0f / m_AlphaMaskFBO->GetSpecification().Width;
    m_TexelH = 1.0f / m_AlphaMaskFBO->GetSpecification().Height;
}
void PPOutline::OnWindowResize(const WindowResizeEvent &e) {}

void PPOutline::Execute(FrameData *data) {
    /* Render to the alpha mask */
    m_AlphaMaskFBO->Bind();
    m_AlphaMaskFBO->Clear();
    m_Renderer.Open(data->SceneCamera);
    for (const auto &sprite : data->PPSpriteOutline) {
        m_Renderer.DrawQuad({sprite.Center, sprite.Size, sprite.Texture,
                             sprite.Color, sprite.RotationZ});
    }
    m_Renderer.Close();
    m_AlphaMaskFBO->Unbind();

    /* Now that we have the alpha mask, we can draw the outline */
    m_OutlineShader->Bind();
    m_OutlineShader->SetFloat2("u_TexelSize", {m_TexelW, m_TexelH});
    m_OutlineShader->SetFloat("u_Thickness", 4.0f);
    m_OutlineShader->SetColor("u_OutlineColor", Colors::White);
    m_AlphaMaskFBO->GetColorTextures()[0]->Bind(0);
    API->DrawArrays(m_VAO, 6);
    m_OutlineShader->Unbind();
}
} // namespace Mana
