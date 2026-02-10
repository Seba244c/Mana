#pragma once
#include "mpch.h"

#include "Component.h"

namespace Mana {
class Core {
  public:
    Core(flecs::world &world);
};

struct WorldPosition : glm::vec3 {
    COMPONENT_NAME(WorldPosition);

    WorldPosition() : glm::vec3(0.0f) {} // Default init
    WorldPosition(float x_, float y_, float z_) : glm::vec3(x_, y_, z_) {}
    WorldPosition(glm::vec3 v) : glm::vec3(v) {}

    static void Describe(flecs::component<WorldPosition> c) {
        c.member<float>("x");
        c.member<float>("y");
        c.member<float>("z");
    }
};

struct Position : glm::vec3 {
    COMPONENT_NAME(Position);
    COMPONENT_ENSURE(WorldPosition);

    Position() : glm::vec3(0.0f) {} // Default init
    Position(float x_, float y_, float z_) : glm::vec3(x_, y_, z_) {}
    Position(glm::vec3 v) : glm::vec3(v) {}

    static void Describe(flecs::component<Position> c) {
        c.member<float>("x");
        c.member<float>("y");
        c.member<float>("z");
    }
};

struct WorldScale : glm::vec3 {
    COMPONENT_NAME(WorldScale);

    WorldScale() : glm::vec3(1.0f) {} // Default init
    WorldScale(float x_, float y_, float z_) : glm::vec3(x_, y_, z_) {}
    WorldScale(glm::vec3 v) : glm::vec3(v) {}

    static void Describe(flecs::component<WorldScale> c) {
        c.member<float>("x");
        c.member<float>("y");
        c.member<float>("z");
    }
};

struct Scale : glm::vec3 {
    COMPONENT_NAME(Scale);
    COMPONENT_ENSURE(WorldScale);

    Scale() : glm::vec3(1.0f) {} // Default init
    Scale(float x_, float y_, float z_) : glm::vec3(x_, y_, z_) {}
    Scale(glm::vec3 v) : glm::vec3(v) {}

    static void Describe(flecs::component<Scale> c) {
        c.member<float>("x");
        c.member<float>("y");
        c.member<float>("z");
    }
};

struct WorldRotation : glm::vec3 {
    COMPONENT_NAME(WorldRotation);

    WorldRotation() : glm::vec3(0.0f) {} // Default init
    WorldRotation(float x_, float y_, float z_) : glm::vec3(x_, y_, z_) {}
    WorldRotation(glm::vec3 v) : glm::vec3(v) {}

    static void Describe(flecs::component<WorldRotation> c) {
        c.member<float>("x");
        c.member<float>("y");
        c.member<float>("z");
    }
};

struct Rotation : glm::vec3 {
    COMPONENT_NAME(Rotation);
    COMPONENT_ENSURE(WorldRotation);

    Rotation() : glm::vec3(0.0f) {} // Default init
    Rotation(float x_, float y_, float z_) : glm::vec3(x_, y_, z_) {}
    Rotation(glm::vec3 v) : glm::vec3(v) {}

    static void Describe(flecs::component<Rotation> c) {
        c.member<float>("x");
        c.member<float>("y");
        c.member<float>("z");
    }
};
} // namespace Mana
