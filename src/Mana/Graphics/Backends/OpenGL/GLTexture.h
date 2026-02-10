#pragma once
#include "mpch.h"

#include "Mana/Graphics/Texture.h"

namespace Mana {
typedef GLType TextureID;

class GLTexture final : public Texture {
  public:
    GLTexture(const ImageData *data);
    GLTexture(uint32_t width, uint32_t height, GLenum internalFormat,
              FilteringMethod mag, FilteringMethod min);
    ~GLTexture();

    [[nodiscard]] uint32_t GetWidth() const override { return m_Width; }
    [[nodiscard]] uint32_t GetHeight() const override { return m_Height; }
    [[nodiscard]] uint32_t GetRendererID() const override {
        return m_RendererID;
    }
    void Bind(uint32_t slot) const override;

  private:
    uint32_t m_Width, m_Height;
    TextureID m_RendererID;
};
} // namespace Mana
