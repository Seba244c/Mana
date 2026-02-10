#include "Mana/Physics/Physics2D.h"
#include "Mana/World/ECS.h"

#include "Physics.h"

namespace Mana {
Physics::Physics(flecs::world &world) {
    // Register components
    ECSReflect::ComponentRegistrator<TypesList<
#if M_DEBUG
        DebugPhysicsConfig
#endif
        >>
        components;
    components.RegisterAll(world);
    world.event<CollisionEvent>();

#if M_DEBUG
    world.add<DebugPhysicsConfig>();
#endif

    // Import submodules
    world.import <Physics2D>();
}
} // namespace Mana
