#pragma once

#include "Mana/Math/Random.h"
#include "Mana/World/Core.h"
#include "Mana/World/ECS.h"

#include "Core.hpp"
#include <Mana.h>
#include <flecs.h>
#include <flecs/addons/cpp/mixins/pipeline/decl.hpp>

namespace Game {
struct BackgroundManager {
    COMPONENT_NAME(BackgroundManager);
    float MoveSpeed = 5;
    float Width;
    uint8_t Count;

    static void RegisterSystem(flecs::world &world) {
        world.system<BackgroundManager>()
            .kind(Mana::ECS::Phases.OnUpdate)
            .each([](flecs::entity e, BackgroundManager &b) {
                e.children([b](flecs::entity c) {
                    auto &pos = c.get_mut<Mana::Position>();
                    pos.x -= b.MoveSpeed * c.world().delta_time();
                    if (pos.x < (b.Count * b.Width * -0.5f))
                        pos.x += b.Count * b.Width;
                    c.modified<Mana::Position>();
                });
            });
    }

    static void Describe(flecs::component<BackgroundManager> c) {
        c.add(flecs::CanToggle);
    }
};

struct PipeManager {
    COMPONENT_NAME(PipeManager);
    COMPONENT_ENSURE(Mana::Position);
    float MoveSpeed = 5;
    float Width;
    uint8_t Count;
    static constexpr const int SpawnRange = 5;

    static void Describe(flecs::component<PipeManager> c) {
        c.add(flecs::CanToggle);
    }

    static void RegisterSystem(flecs::world &world) {
        world.system<PipeManager>()
            .kind(Mana::ECS::Phases.OnUpdate)
            .each([](flecs::entity e, PipeManager &p) {
                e.children([p](flecs::entity c) {
                    auto &pos = c.get_mut<Mana::Position>();
                    pos.x -= p.MoveSpeed * c.world().delta_time();
                    if (pos.x < (p.Count * p.Width * -0.5f)) {
                        pos.x += p.Count * p.Width;
                        pos.y = Mana::Random::Int(-SpawnRange, SpawnRange);
                        c.get_mut<Point>().Scored = false;
                    }
                    c.modified<Mana::Position>();
                });
            });
    }
};

inline flecs::entity PPipe(const flecs::world &world, flecs::entity pipes,
                           int x) {
    auto pipe =
        world.entity()
            .child_of(pipes)
            .set(Mana::Position(x,
                                Mana::Random::Int(-PipeManager::SpawnRange,
                                                  PipeManager::SpawnRange),
                                0))
            .set(Mana::Scale(2, 4, 0))
            .add<Point>()
            .set(Mana::BoxCollider(true));

    world.entity()
        .set(Mana::Position(0, -8, 0))
        .set(Mana::Scale(2, 12, 0))
        .set(Mana::BoxCollider())
        .add<Death>()
        .set(Mana::Sprite{M_ASSET(Mana::Texture, "pipe.png")})
        .child_of(pipe);
    world.entity()
        .set(Mana::Position(0, 9, 0))
        .set(Mana::Scale(2, 14, 0))
        .set(Mana::BoxCollider())
        .add<Death>()
        .set(Mana::Rotation(0, 0, glm::pi<float>()))
        .set(Mana::Sprite{M_ASSET(Mana::Texture, "pipe.png")})
        .child_of(pipe);
    return pipe;
}
} // namespace Game
