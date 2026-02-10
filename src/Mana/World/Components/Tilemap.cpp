#include "Tilemap.h"

#include "Mana/Core/Application.h"
#include "Mana/World/Core.h"
#include "Mana/World/ECS.h"

namespace Mana {
TilemapChunk TilemapChunk::Decode(Utils::Files::BufferReader &reader) {
    std::array<std::array<uint8_t, 8>, 8> Grid;
    for (int i = 0; i < 8; i++) {
        Grid[i] =
            std::bit_cast<std::array<uint8_t, 8>>(reader.Read<uint64_t>());
    }

    return TilemapChunk(Grid);
}

void TilemapChunk::Encode(Utils::Files::BufferWriter &writer) const {
    for (int i = 0; i < 8; i++) {
        writer.Write<uint64_t>(std::bit_cast<int64_t>(Grid[i]));
    }
}

TilemapChunkGrid TilemapChunkGrid::Decode(Utils::Files::BufferReader &reader) {
    float spriteSize = reader.Read<float>();
    float renderRadius = reader.Read<float>();

    TilemapChunkGrid out(spriteSize, renderRadius);
    size_t count = reader.Read<size_t>();
    for (size_t i = 0; i < count; i++) {
        ChunkKey key = reader.Read<ChunkKey>();
        out.Chunks[key] = TilemapChunk::Decode(reader);
    }

    return {};
}

void TilemapChunkGrid::Encode(Utils::Files::BufferWriter &writer) const {
    writer.Write<float>(m_SpriteSize);
    writer.Write<float>(m_RenderRadiusChunk);

    writer.Write<size_t>(Chunks.size());
    for (const auto &chunk : Chunks) {
        writer.Write<ChunkKey>(chunk.first);
        chunk.second.Encode(writer);
    }
}

Tilemaps::Tilemaps(flecs::world &world) {
    ECSReflect::ComponentRegistrator<TypesList<TilemapChunkGrid, Tilemap>>
        components;
    components.RegisterAll(world);

    world
        .system<const WorldPosition, const Tilemap, TilemapChunkGrid>(
            "Render Tilemaps")
        .kind(ECS::Phases.OnRender)
        .each([](flecs::entity, const WorldPosition &pos,
                 const Tilemap &tilemap, TilemapChunkGrid &grid) {
            // Render a 3x3 bounds
            glm::ivec2 center =
                grid.ChunkCoords(Application::Instance()
                                     ->GetGraphicsPipeline()
                                     .FrameData()
                                     .SceneCamera->GetPosition());

            for (int dy = -grid.RenderRadiusChunk();
                 dy <= grid.RenderRadiusChunk(); dy++) {
                for (int dx = -grid.RenderRadiusChunk();
                     dx <= grid.RenderRadiusChunk(); dx++) {
                    glm::ivec2 c = center + glm::ivec2(dx, dy); // The chunk
                    glm::vec2 origin = pos;
                    origin += grid.ChunkOrigin(c.x, c.y);

                    Application::Instance()
                        ->GetGraphicsPipeline()
                        .FrameData()
                        .TilemapChunks.emplace_back(
                            grid.Chunk(grid.GridKey(c.x, c.y)), origin,
                            grid.SpriteSize(), tilemap.SpriteSheet.get());
                }
            }
        });
}
} // namespace Mana
