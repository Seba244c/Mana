#pragma once

#include <Mana.h>

#include "Mana/Core/Application.h"
#include "Mana/Core/Core.h"
#include "Mana/World/ECS.h"

#include "Core.hpp"

namespace Game {
struct TappyController {
    COMPONENT_NAME(TappyController);
    COMPONENT_ENSURE(Mana::Position, Mana::Rotation);

    static constexpr float GRAVITY_SPEED = 30;
    static constexpr float JUMP_ENERGY = 0.5f;
    static constexpr float VELOCITY_DROPFF = 4;
    static constexpr float ROTATION_BOUNDS =
        (glm::pi<float>() / 6.0f); // 30 degrees in radians
    float Velocity;
    Mana::Ref<Mana::Sound> SoundJump;
    Mana::Ref<Mana::Sound> SoundDeath;

    TappyController() {
        SoundJump = M_ASSET(Mana::Sound, "tappy_jump.mp3");
        SoundDeath = M_ASSET(Mana::Sound, "tappy_death.mp3");
    }

    static void Describe(flecs::component<TappyController> c) {
        c.add(flecs::CanToggle);
    }

    static void Register(flecs::world &world) {
        Mana::ECSReflect::ComponentRegistrator<Mana::TypesList<TappyController>>
            c;
        c.RegisterAll(world);

        world
            .system<TappyController, Mana::Position, Mana::Rotation>(
                "Player Movement")
            .kind(Mana::ECS::Phases.OnUpdate)
            .each([&](flecs::entity e, TappyController &c, Mana::Position &p,
                      Mana::Rotation &r) {
                auto dt = e.world().delta_time();
                if (Mana::Input::IsKeyPressed(Mana::Key::SPACE)) {
                    c.Velocity = JUMP_ENERGY * Mana::Random::Float(0.5f, 1.2f);
                    Mana::Application::Instance()->GetAudio().PlaySound(
                        Game::s_ChannelSFX, c.SoundJump);
                } else if (c.Velocity <= 0)
                    c.Velocity = std::max(
                        c.Velocity -
                            ((VELOCITY_DROPFF * (-c.Velocity + 0.1f)) * dt),
                        -1.0f);
                else
                    c.Velocity = std::min(
                        c.Velocity -
                            ((VELOCITY_DROPFF * (c.Velocity + 0.5f)) * dt),
                        1.5f);

                r.z = c.Velocity * -ROTATION_BOUNDS;
                p.y += GRAVITY_SPEED * c.Velocity * dt;
                e.modified<Mana::Rotation>();
                e.modified<Mana::Position>();
            });
    }
};
} // namespace Game
