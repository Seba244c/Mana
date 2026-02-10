#pragma once
#include "mpch.h"

#include "Mana/World/Component.h"

namespace Mana {
struct CollisionEvent {
    const flecs::entity Other;
};

#if M_DEBUG
struct DebugPhysicsConfig {
    COMPONENT_NAME(PhysicsDebugger)
    bool DrawColliders = false;
    bool DrawVelocities = true;
    bool DrawMouseCameraDirection = true;

    Color ColorColliders = {0, 1, 0, 1};
    Color ColorVelocities = {1, 1, 0, 1};

    static void Describe(flecs::component<DebugPhysicsConfig> c) {
        c.member<bool>("DrawColliders");
        c.member<bool>("DrawVelocities");
        c.member<bool>("DrawMouseCameraDirection");
        c.member<Color>("ColliderColor");
        c.member<Color>("VelocityColor");
    }
};
#endif

class Physics {
  public:
    Physics(flecs::world &world);
};
} // namespace Mana
