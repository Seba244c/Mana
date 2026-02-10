#include "Physics2D.h"

#include "Mana/Physics/Physics.h"
#include "Mana/World/Components/Chunk.h"
#include "Mana/World/ECS.h"

namespace Mana {
Physics2D::Physics2D(flecs::world &world) {
    ECSReflect::ComponentRegistrator<TypesList<
        Collider2D, BoxCollider, CircleCollider, Collision, RigidBody2D>>
        components;
    components.RegisterAll(world);

    world.observer<BoxCollider>("Add to SpatialGrid2D")
        .event(flecs::Monitor)
        .with<WorldPosition>()
        .in()
        .each([](flecs::iter &it, size_t i, BoxCollider c) {
            auto &grid = it.world().get_mut<EntityChunkGrid>();
            auto p = it.entity(0).get<WorldPosition>();

            if (it.event() == flecs::OnAdd) {
                // Add to grid
                c.GridKey = grid.ChunkAt(p);
                grid.Chunk(c.GridKey).AddEntity(it.entity(0));
            } else if (it.event() == flecs::OnRemove) {
                grid.Chunk(c.GridKey).RemoveEntity(it.entity(0));
            }
        });

    world.observer<const WorldPosition, Collider2D>("Update SpatialGrid2D")
        .event(flecs::OnSet)
        .in()
        .each([](flecs::entity e, const WorldPosition &p, Collider2D &c) {
            auto &grid = e.world().get_mut<EntityChunkGrid>();

            int64_t key = grid.ChunkAt(p);

            if (key != c.GridKey) {
                grid.Chunk(c.GridKey).RemoveEntity(e);
                c.GridKey = key;
                grid.Chunk(c.GridKey).AddEntity(e);
            }
        });

    // Collision Finding
    world
        .system<const RigidBody2D, const WorldPosition, const BoxCollider,
                const WorldScale, Collision>("RigidBody Collision (Box)")
        .kind(ECS::Phases.OnValidate)
        .immediate(true)
        .in()
        .each([](flecs::entity e, const RigidBody2D &r, const Position &p,
                 const BoxCollider &c, const WorldScale &s,
                 Collision &collision) {
            std::vector<flecs::entity> cell =
                e.world().get_mut<EntityChunkGrid>().EntitiesInRange(p, 1);

            collision.moveX = 0;
            collision.moveY = 0;
            collision.hasCollision = false;
            for (flecs::entity b : cell) {
                if (e == b)
                    continue;
                if (e.has<BoxCollider>()) {
                    auto bC = b.get<BoxCollider>();
                    auto bP = b.get<WorldPosition>();
                    auto bS = b.get<WorldScale>();
                    glm::vec2 bH = 0.5f * bS;
                    glm::vec2 aH = 0.5f * s;
                    float dx = p.x - bP.x;
                    float px = aH.x + bH.x - std::abs(dx);
                    if (px <= 0)
                        continue;
                    float dy = p.y - bP.y;
                    float py = aH.y + bH.y - std::abs(dy);
                    if (py <= 0)
                        continue;

                    e.emit<CollisionEvent>({b});
                    b.emit<CollisionEvent>({e});

                    if (!r.IsStatic && !bC.IsTrigger) {
                        collision.hasCollision = true;
                        if (px < py) {
                            collision.moveX += (dx > 0 ? px : -px);
                        } else {
                            collision.moveY += (dy > 0 ? py : -py);
                        }
                    }
                }
            }
        });
    world.system<Position, const Collision>("Handle Collisions")
        .kind(ECS::Phases.PostUpdate)
        .immediate(true)
        .out()
        .each([](flecs::entity e, Position &p, const Collision &c) {
            if (!c.hasCollision)
                return;
            p.x += c.moveX;
            p.y += c.moveY;
            e.modified<Position>();
        });

// Debug Renderers
#if 0
#if M_DEBUG
    world
        .system<const WorldPosition, const BoxCollider, const WorldScale>(
            "DebugRenderBoxColliders")
        .kind(
            ECS::Phases.OnDebugRender) // After all updates but before rendering
        .in()
        .each([](flecs::entity e, const WorldPosition &position,
                 const BoxCollider &collider, const WorldScale &scale) {
            auto debugConfig = e.world().get<DebugPhysicsConfig>();

            if (debugConfig.DrawColliders) {
                glm::vec3 halfScale = scale * 0.5f;

                glm::vec2 tr = position + halfScale;
                glm::vec2 bl = position - halfScale;
                glm::vec2 tl = {bl.x, tr.y};
                glm::vec2 br = {tr.x, bl.y};

                Renderer2D::Instance().DrawLine(
                    {tl, tr, debugConfig.ColorColliders, 0.07f});
                Renderer2D::Instance().DrawLine(
                    {tr, br, debugConfig.ColorColliders, 0.07f});
                Renderer2D::Instance().DrawLine(
                    {br, bl, debugConfig.ColorColliders, 0.07f});
                Renderer2D::Instance().DrawLine(
                    {bl, tl, debugConfig.ColorColliders, 0.07f});
            }
        });
    world.system<const WorldPosition, const Collision>("DebugRenderCollisions")
        .kind(
            ECS::Phases.OnDebugRender) // After all updates but before rendering
        .in()
        .each([](flecs::entity e, const WorldPosition &position,
                 const Collision &c) {
            if (!c.hasCollision)
                return;
            auto debugConfig = e.world().get<DebugPhysicsConfig>();

            if (debugConfig.DrawVelocities) {
                glm::vec2 n = position;
                glm::vec2 d = 4.0f * glm::vec2{c.moveX, c.moveY};
                n += d;

                Renderer2D::Instance().DrawLine(
                    {position, n, debugConfig.ColorVelocities, 0.1f});
            }
        });
    world
        .system<const MouseDirTracker, const WorldPosition>(
            "DebugRenderMouseCameraDirs")
        .kind(
            ECS::Phases.OnDebugRender) // After all updates but before rendering
        .in()
        .each([](flecs::entity e, const MouseDirTracker &mdt,
                 const WorldPosition &wp) {
            auto debugConfig = e.world().get<DebugPhysicsConfig>();

            if (debugConfig.DrawMouseCameraDirection) {
                glm::vec2 n = wp;
                n += mdt.MouseDirection * 1.2f;

                Renderer2D::Instance().DrawLine(
                    {wp, n, debugConfig.ColorVelocities, 0.1f});
            }
        });

#endif
#endif
}
} // namespace Mana
