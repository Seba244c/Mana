#pragma once
#include <mpch.h>

#include "Mana/Graphics/Texture.h"

namespace Mana {
enum class FramebufferTextureFormat {
    None = 0,

    // Color Formats
    RGBA8,
    R8,

    // Depth & Stencil formats
    DEPTH24STENCIL8,

    // Defaults
    Depth = DEPTH24STENCIL8,
    Color = RGBA8
};

struct FramebufferTextureSpecification {
    FramebufferTextureSpecification() = default;
    FramebufferTextureSpecification(FramebufferTextureFormat format)
        : TextureFormat(format) {}

    FramebufferTextureFormat TextureFormat = FramebufferTextureFormat::None;
};

struct FramebufferAttachmentSpecification {
    FramebufferAttachmentSpecification() = default;
    FramebufferAttachmentSpecification(
        const std::initializer_list<FramebufferTextureSpecification>
            attachments)
        : Attachments(attachments) {}

    std::vector<FramebufferTextureSpecification> Attachments;
};

struct FramebufferSpecification {
    uint32_t Width = 1, Height = 1;
    FramebufferAttachmentSpecification Attachments;
};

class Framebuffer {
  public:
    virtual ~Framebuffer() = default;

    virtual void Bind() = 0;
    virtual void Unbind() = 0;
    virtual void Clear() = 0;
    virtual const std::vector<Ref<Texture>> &GetColorTextures() = 0;
    virtual const Ref<Texture> &GetDepthTexture() = 0;

    virtual void Resize(uint32_t width, uint32_t height) = 0;
    [[nodiscard]] virtual const FramebufferSpecification &
    GetSpecification() const = 0;
};
} // namespace Mana
