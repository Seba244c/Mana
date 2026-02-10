#pragma once

#include "mpch.h"

namespace Mana {
struct Phases {
    const flecs::entity_t PreUpdate =
        flecs::PreUpdate; // Load inputs, data and such
    const flecs::entity_t OnUpdate = flecs::OnUpdate; // Do the update
    flecs::entity_t OnValidate;                       // Validate
    flecs::entity_t PostUpdate;                       // Last Resolution phase
    flecs::entity_t PreRender;
    flecs::entity_t OnRender;
    flecs::entity_t OnDebugRender;
    flecs::entity_t PostRender;
};

class ECSRegistry {
  public:
    template <typename T>
    flecs::component<T> RegisterComponentByType(flecs::world &world,
                                                const char *name) {
        // Register with flecs
        auto component = world.component<T>(name);
        // component.set_name(name);

        return component;
    }
};

class ECS {
  public:
    static void Init(flecs::world &world);

  public:
    static Phases Phases;
    static ECSRegistry Registry;
};

namespace ECSReflect {
template <typename T>
concept HasComponentNameFunction = requires {
    { T::ComponentName() } -> std::convertible_to<const char *>;
};

template <typename T> const char *ComponentName() {
    if constexpr (HasComponentNameFunction<T>) {
        return T::ComponentName();
    } else {
        return typeid(T).name();
    }
}

template <typename T>
concept HasDescribeFunction = requires(T t, flecs::component<T> c) {
    { T::Describe(c) };
};

template <typename T>
concept HasEnsuredComponents = requires {
    { typename T::EnsuredComponents() };
};

// Primary component registry template
template <typename Components = TypesList<>> class ComponentRegistrator {
  private:
    // Helper to register a single component
    template <typename T> static void RegisterComponent(flecs::world &world) {
        const char *name = ComponentName<T>();

        auto component = ECS::Registry.RegisterComponentByType<T>(world, name);
        MC_TRACE(" - {}", name);

        // Register members if available
        if constexpr (HasDescribeFunction<T>) {
            T::Describe(component);
        }

        // Setup ensured components if defined
        if constexpr (HasEnsuredComponents<T>) {
            register_ensured_components<T>(world);
        }
    }

    template <typename... Ts>
    static void register_components_impl(flecs::world &world,
                                         TypesList<Ts...>) {
        (RegisterComponent<Ts>(world), ...);
    }

    // Helper to ensure dependencies for a specific component
    template <typename T, typename EnsuredList>
    static void ensure_dependencies(flecs::entity e) {
        if constexpr (is_type_list<EnsuredList>::value) {
            ensure_type_list_dependencies<EnsuredList>(e);
        } else {
            e.ensure<EnsuredList>();
        }
    }

    // Register components that should be ensured
    template <typename T>
    static void register_ensured_components(flecs::world &world) {
        using EnsuredList = typename T::EnsuredComponents;

        // Create an observer that automatically adds the ensured components
        world.observer<T>()
            .event(flecs::OnAdd)
            .with(flecs::Disabled)
            .oper(flecs::Optional)
            .with(flecs::Prefab)
            .oper(flecs::Optional)
            .each([](flecs::entity e, T &) {
                ensure_dependencies<T, EnsuredList>(e);
            });
    }

    // Helper to check if a type is a type_list
    template <typename T> struct is_type_list : std::false_type {};

    template <typename... Ts>
    struct is_type_list<TypesList<Ts...>> : std::true_type {};

    // Helper to ensure dependencies from a type_list
    template <typename TL>
    static void ensure_type_list_dependencies(flecs::entity e) {
        if constexpr (is_type_list<TL>::value) {
            ensure_type_list_dependencies_impl(e, TL{});
        }
    }

    template <typename... Ts>
    static void ensure_type_list_dependencies_impl(flecs::entity e,
                                                   TypesList<Ts...>) {
        (e.ensure<Ts>(), ...);
    }

  public:
    // Register all components in the type list
    static void RegisterAll(flecs::world &world) {
        MC_TRACE("Registering Component Registry:");
        register_components_impl(world, Components{});
    }

    // Add a component type to the registry
    template <typename T>
    using with = ComponentRegistrator<TypesList<T, Components>>;
};
} // namespace ECSReflect
} // namespace Mana
