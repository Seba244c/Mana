#include "Chunk.h"

#include "Mana/IO/Input.h"
#include "Mana/Rendering/Renderer.h"
#include "Mana/World/ECS.h"

namespace Mana {
#if 0
#if M_DEBUG
static bool s_DebugRenderChunks = false;
static const uint s_DebugChunkRadius = 2;
static const Color s_DebugChunkColor = Colors::Blue;

template <typename T>
static void DrawChunkBounds(const ChunkGrid2D<T> &grid, int32_t cx,
                            int32_t cy) {
    glm::vec2 origin = grid.ChunkOrigin(cx, cy);
    float s = grid.GetChunkSize();

    // Four corners
    glm::vec3 a = {origin.x, origin.y, 0.f};
    glm::vec3 b = {origin.x + s, origin.y, 0.f};
    glm::vec3 c = {origin.x + s, origin.y + s, 0.f};
    glm::vec3 d = {origin.x, origin.y + s, 0.f};

    Renderer2D::Instance().DrawLine({a, b, s_DebugChunkColor});
    Renderer2D::Instance().DrawLine({b, c, s_DebugChunkColor});
    Renderer2D::Instance().DrawLine({c, d, s_DebugChunkColor});
    Renderer2D::Instance().DrawLine({d, a, s_DebugChunkColor});
}

template <typename T>
static void DebugRenderChunks(const ChunkGrid2D<T> &grid, int radius,
                              const glm::vec3 &pos) {
    glm::ivec2 center = grid.ChunkCoords(pos);

    // Render a 3x3 bounds
    for (int dy = -radius; dy <= radius; dy++) {
        for (int dx = -radius; dx <= radius; dx++) {
            glm::ivec2 c = center + glm::ivec2(dx, dy);
            DrawChunkBounds(grid, c.x, c.y);
        }
    }
}
#endif
#endif

Chunk::Chunk(flecs::world &world) {
    // Register components
    ECSReflect::ComponentRegistrator<TypesList<EntityChunkGrid>> components;
    components.RegisterAll(world);

    world.add<EntityChunkGrid>();

#if 0
#if M_DEBUG
    world.system<EntityChunkGrid>("Debug: Render Entity Chunks")
        .kind(ECS::Phases.OnDebugRender)
        .run([](flecs::iter &it) {
            it.fini();
            if (Input::IsKeyPressed(Key::F4))
                s_DebugRenderChunks = !s_DebugRenderChunks;

            const auto &grid = it.world().get<EntityChunkGrid>();
            if (s_DebugRenderChunks)
                DebugRenderChunks(
                    grid, s_DebugChunkRadius,
                    Renderer2D::Instance().GetActiveCamera()->GetPosition());
        });
#endif
#endif
}
} // namespace Mana
