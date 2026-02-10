#pragma once
#include <mpch.h>

#include "Mana/Core/Asset.h"
#include "Mana/Graphics/Texture.h"

namespace Mana {
class SpriteSheet {
  public:
    struct ShtSprite {
        std::string Name;
        glm::uvec2 Pos;
        glm::uvec2 Size;
        std::array<glm::vec2, 4> TexCoords;
    };

  private:
    enum ChunkType {
        Metadata = 0,
        Sprite = 1,
    };

    struct MetaData {
        uint64_t TextureHashID;
    };

    static std::vector<uint8_t> SpriteToBinary(const uint8_t id,
                                               ShtSprite sprite);
    static ShtSprite BinaryToSprite(uint8_t &id, std::vector<uint8_t> data);

  public:
    static void CreateMote(std::filesystem::path src,
                           std::vector<MoteChunkOwned> &data);
    static Ref<SpriteSheet> Create(const MoteData &data);

  public:
    SpriteSheet(Ref<Texture> texture, std::map<uint8_t, ShtSprite> &&sprites);

    const Ref<Texture> &GetTexture() const { return m_Texture; }
    const std::array<glm::vec2, 4> &TexCoords(uint8_t sprite) const {
        return m_Sprites.at(sprite).TexCoords;
    }

  private:
    const Ref<Texture> m_Texture;
    std::map<uint8_t, ShtSprite> m_Sprites;
};
} // namespace Mana
