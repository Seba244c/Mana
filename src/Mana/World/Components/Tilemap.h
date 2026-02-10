#pragma once
#include <mpch.h>

#include "Chunk.h"

#include "Mana/Core/Utils.h"
#include "Mana/Rendering/SpriteSheet.h"
#include "Mana/World/Core.h"

namespace Mana {
class Tilemaps {
  public:
    Tilemaps(flecs::world &world);
};

struct TilemapChunk {
    std::array<std::array<uint8_t, 8>, 8>
        Grid; // Indexed x then y, from the bl corner of the chunk

    static TilemapChunk Decode(Utils::Files::BufferReader &reader);
    void Encode(Utils::Files::BufferWriter &writer) const;
};

class TilemapChunkGrid : public ChunkGrid2D<TilemapChunk> {
  public:
    COMPONENT_NAME(TilemapChunkGrid);
    TilemapChunkGrid(float spriteSize = 1.0f, float renderRadiusChunk = 2)
        : ChunkGrid2D(spriteSize * 8), m_SpriteSize(spriteSize),
          m_RenderRadiusChunk(renderRadiusChunk) {}

    float SpriteSize() const { return m_SpriteSize; }
    float RenderRadiusChunk() const { return m_RenderRadiusChunk; }

    static TilemapChunkGrid Decode(Utils::Files::BufferReader &reader);
    void Encode(Utils::Files::BufferWriter &writer) const;

  private:
    float m_SpriteSize;
    float m_RenderRadiusChunk;
};

class Tilemap {
  public:
    COMPONENT_NAME(Tilemap);
    COMPONENT_ENSURE(TilemapChunkGrid, Position);

    Ref<SpriteSheet> SpriteSheet;
};
} // namespace Mana
