#pragma once

#include "mpch.h"

#include "Mana/World/Component.h"
#include "Mana/World/Core.h"

namespace Mana {
struct Collider2D {
    int64_t GridKey;
    bool IsTrigger;

    Collider2D(bool isTrigger = false) : GridKey(0), IsTrigger(isTrigger) {}

    static void Describe(flecs::component<Collider2D> c) {
        c.member<int64_t>("GridKey");
        c.member<bool>("IsTrigger");
    }
};

struct BoxCollider : Collider2D {
    COMPONENT_NAME(BoxCollider);
    COMPONENT_ENSURE(Position, Scale);

    BoxCollider(bool isTrigger = false) : Collider2D(isTrigger) {}

    static void Describe(flecs::component<BoxCollider> c) {
        c.is_a<Collider2D>();
    }
};

struct CircleCollider : Collider2D {
    COMPONENT_NAME(CircleCollider);
    COMPONENT_ENSURE(Position, Scale);

    float Radius;

    CircleCollider(float radius = 1, bool isTrigger = false)
        : Collider2D(isTrigger), Radius(radius) {}

    static void Describe(flecs::component<BoxCollider> c) {
        c.is_a<Collider2D>();
        c.member<float>("Radius");
    }
};

struct Collision {
    float moveX, moveY;
    bool hasCollision;
};

struct RigidBody2D {
    COMPONENT_NAME(RigidBody2D);
    COMPONENT_ENSURE(Position, Collision);

    bool IsStatic;

    static void Describe(flecs::component<RigidBody2D> c) {
        c.member<bool>("IsStatic");
    }
};

class Physics2D {
  public:
    Physics2D(flecs::world &world);
};
} // namespace Mana
