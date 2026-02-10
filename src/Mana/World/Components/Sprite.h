#pragma once

#include "mpch.h"

#include "Mana/Graphics/Texture.h"
#include "Mana/World/Component.h"
#include "Mana/World/Core.h"

namespace Mana {
struct Sprite {
    COMPONENT_NAME(Sprite);
    COMPONENT_ENSURE(Position, Scale, Rotation);

    Ref<Texture> Texture = nullptr;
    Color Tint = Colors::White;
    int8_t Layer = 0;

    static void Describe(flecs::component<Sprite> c) {
        c.member<Color>("Tint", 1, offsetof(Sprite, Tint));
        c.member<int8_t>("Layer", 1, offsetof(Sprite, Layer));
        c.add(flecs::CanToggle);
    }
};

struct PPSpriteOutline {
    COMPONENT_NAME(PPSpriteOutline);
};

struct SpriteScale {
    COMPONENT_NAME(SpriteScale);
    COMPONENT_ENSURE(Sprite, Scale);

    SpriteScale(float scale = 1.0f) : Scale(scale) {}

    float Scale;

    static void Describe(flecs::component<SpriteScale> c) {
        c.member<float>("Scale");
    }
};

class SpriteRendering {
  public:
    SpriteRendering(flecs::world &world);
};
} // namespace Mana
