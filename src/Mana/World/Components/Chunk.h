#pragma once
#include <mpch.h>

namespace Mana {
class Chunk {
  public:
    Chunk(flecs::world &world);
};

typedef int64_t ChunkKey;

template <typename T> class ChunkGrid2D {
  public:
    ChunkGrid2D(float chunkSize = 10.0f) : m_ChunkSize(chunkSize) {}
    [[nodiscard]] float GetChunkSize() const { return m_ChunkSize; };

  protected:
    std::unordered_map<ChunkKey, T> Chunks;
    float m_ChunkSize;

  public:
    // Hash function for grid coordinates
    [[nodiscard]] static ChunkKey GridKey(int x, int y) {
        return (int64_t(x) << 32) | (uint32_t(y));
    }

    [[nodiscard]] glm::ivec2 ChunkCoords(const glm::vec3 &pos) const {
        return {static_cast<int>(std::floor(pos.x / m_ChunkSize)),
                static_cast<int>(std::floor(pos.y / m_ChunkSize))};
    }

    [[nodiscard]] glm::vec2 ChunkOrigin(int cx, int cy) const {
        return glm::vec2(cx * m_ChunkSize, cy * m_ChunkSize);
    }

    [[nodiscard]] ChunkKey ChunkAt(float x, float y) const {
        // Transform into grid coordinates
        x = static_cast<int>(std::floor(x / m_ChunkSize));
        y = static_cast<int>(std::floor(y / m_ChunkSize));
        return GridKey(x, y);
    }
    [[nodiscard]] ChunkKey ChunkAt(const glm::vec3 pos) const {
        return ChunkAt(pos.x, pos.y);
    }

    [[nodiscard]] T &Chunk(const ChunkKey chunkId) { return Chunks[chunkId]; }
};

struct EntityChunk {
    std::vector<flecs::entity> Entities;

    void AddEntity(flecs::entity e) { Entities.push_back(e); }

    void RemoveEntity(flecs::entity e) {
        auto it = std::find(Entities.begin(), Entities.end(), e);
        if (it != Entities.end()) {
            Entities.erase(it);
        }
    }
};

class EntityChunkGrid : public ChunkGrid2D<EntityChunk> {
  public:
    COMPONENT_NAME(EntityChunkGrid);
    EntityChunkGrid() : ChunkGrid2D() {}

    [[nodiscard]] std::vector<flecs::entity>
    EntitiesInRange(const glm::vec3 &pos, const int range) {
        M_ASSERT(range >= 1);

        std::vector<flecs::entity> result;
        auto c = ChunkCoords(pos);

        for (int dy = -range; dy <= range; dy++) {
            for (int dx = -range; dx <= range; dx++) {
                auto key = GridKey(c.x + dx, c.y + dy);
                if (Chunks.find(key) != Chunks.end()) {
                    result.insert(result.end(), Chunks[key].Entities.begin(),
                                  Chunks[key].Entities.end());
                }
            }
        }

        return result;
    }
};
} // namespace Mana
