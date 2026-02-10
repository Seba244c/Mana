#pragma once
#include <mpch.h>

#include "Mana/Graphics/Texture.h"
#include "Mana/World/Components/Tilemap.h"

namespace Mana {
struct DrwSprite {
    int8_t Layer;
    glm::vec2 Center;
    glm::vec2 Size;
    const Texture *Texture;
    Color Color;
    float RotationZ;
};

struct DrwTilemapChunk {
    TilemapChunk Chunk;
    glm::vec2 Origin; // origin is the bottom left corner of the chunk

    float SpriteSize;
    const SpriteSheet *Sheet;
};
} // namespace Mana
