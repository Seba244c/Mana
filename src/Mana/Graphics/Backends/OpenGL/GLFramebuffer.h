#pragma once
#include <mpch.h>

#include "Mana/Graphics/Backends/OpenGL/GLTexture.h"
#include "Mana/Graphics/Framebuffer.h"

typedef GLType GLFramebufferID;

namespace Mana {
class GLFramebuffer final : public Framebuffer {
  public:
    GLFramebuffer(const FramebufferSpecification &specs);
    ~GLFramebuffer() override;
    void Invalidate();

    void Bind() override;
    void Unbind() override;
    void Clear() override;
    const std::vector<Ref<Texture>> &GetColorTextures() override {
        return m_ColorAttachments;
    }
    const Ref<Texture> &GetDepthTexture() override { return m_DepthAttachment; }

    void Resize(uint32_t width, uint32_t height) override;

    const FramebufferSpecification &GetSpecification() const override {
        return m_Specification;
    }

  private:
    GLFramebufferID m_RendererID = 0;
    FramebufferSpecification m_Specification;

    // GLFramebuffer() sorts the texturespecification into theese to catagories
    std::vector<FramebufferTextureSpecification>
        m_ColorAttachmentSpecifications;
    FramebufferTextureSpecification m_DepthAttachmentSpecification =
        FramebufferTextureFormat::None;

    // Textures
    std::vector<Ref<Texture>> m_ColorAttachments;
    Ref<Texture> m_DepthAttachment = 0;
};
} // namespace Mana
