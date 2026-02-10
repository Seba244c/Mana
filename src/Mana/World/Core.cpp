#include "Core.h"
#include "Mana/World/ECS.h"

namespace Mana {
struct Types {
    Types(flecs::world &world) {
        using Components = ECSReflect::ComponentRegistrator<TypesList<Color>>;
        Components::RegisterAll(world);

        // External types
        world.component<glm::vec3>()
            .member<float>("x", 1, offsetof(glm::vec3, x))
            .member<float>("y", 1, offsetof(glm::vec3, y))
            .member<float>("z", 1, offsetof(glm::vec3, z));

        world.component<glm::vec2>()
            .member<float>("x", 1, offsetof(glm::vec2, x))
            .member<float>("y", 1, offsetof(glm::vec2, y));
    }
};

template <typename T, typename WT>
void UpdateWorld(flecs::entity e, T &local, WT &world) {
    world = local;

    // Check if it has a parent with WorldPosition
    flecs::entity parent = e.parent();
    if (parent.is_valid()) {
        if (const WT *pwt = parent.try_get<WT>()) {
            world += (glm::vec3)*pwt;
        }
    }

    e.children([&](flecs::entity child) {
        child.modified<T>();
    }); // So they also recalculate

    e.modified<WT>();
}
template <typename T, typename WT>
void UpdateWorldScaled(flecs::entity e, T &local, WT &world) {
    world = local;

    // Check if it has a parent with WorldPosition
    flecs::entity parent = e.parent();
    if (parent.is_valid()) {
        if (const WT *pwt = parent.try_get<WT>()) {
            world *= (glm::vec3)*pwt;
        }
    }

    e.children([&](flecs::entity child) {
        child.modified<T>();
    }); // So they also recalculate

    e.modified<WT>();
}

template <typename T, typename WT>
void RegisterUpdateWorldObserver(flecs::world &world) {
    world.observer<T>((std::string("Update ") + typeid(WT).name()).c_str())
        .event(flecs::OnSet)
        .with(flecs::Disabled)
        .oper(flecs::Optional)
        .each([](flecs::entity e, T &local) {
            UpdateWorld<T, WT>(e, local, e.get_mut<WT>());
        });
}
template <typename T, typename WT>
void RegisterUpdateWorldObserverScaled(flecs::world &world) {
    world.observer<T>((std::string("Update ") + typeid(WT).name()).c_str())
        .event(flecs::OnSet)
        .with(flecs::Disabled)
        .oper(flecs::Optional)
        .each([](flecs::entity e, T &local) {
            UpdateWorldScaled<T, WT>(e, local, e.get_mut<WT>());
        });
}

Core::Core(flecs::world &world) {
    world.module<Core>("Core");
    world.import <Types>();

    ECSReflect::ComponentRegistrator<TypesList<
        WorldPosition, Position, WorldScale, Scale, WorldRotation, Rotation>>
        components;
    components.RegisterAll(world);

    RegisterUpdateWorldObserver<Position, WorldPosition>(world);
    RegisterUpdateWorldObserverScaled<Scale, WorldScale>(world);
    RegisterUpdateWorldObserver<Rotation, WorldRotation>(world);
}
} // namespace Mana
