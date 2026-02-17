#pragma once
#include <mpch.h>

#include "Mana/World/Component.h"

namespace Mana {
/**
 * \brief Represents a single RGBA color with floats bound between 0≤f≤1
 */
struct Color {
    COMPONENT_NAME(Color);

    /** Red component (0≤r≤1) */
    float r;

    /** Green component (0≤r≤1) */
    float g;

    /** Blue component (0≤r≤1) */
    float b;

    /** Alpha component (0≤r≤1) */
    float a;

    constexpr Color(const float r, const float g, const float b,
                    const float a = 1.0f)
        : r(r), g(g), b(b), a(a) {}

    operator glm::vec4() const { return {r, g, b, a}; }

    static void Describe(flecs::component<Color> c) {
        c.member<float>("r");
        c.member<float>("g");
        c.member<float>("b");
        c.member<float>("a");
    }
};

class Colors {
  public:
    static constexpr const auto Transparant = Color(0, 0, 0, 0);
    static constexpr const auto Black = Color(0, 0, 0);
    static constexpr const auto White = Color(1, 1, 1);
    static constexpr const auto Gray = Color(0.5f, 0.5f, 0.5f);
    static constexpr const auto DimGray = Color(0.25f, 0.25f, 0.25f);
    static constexpr const auto DarkGray = Color(0.1f, 0.1f, 0.1f);

    static constexpr const auto Red = Color(1, 0, 0);
    static constexpr const auto Green = Color(0, 1, 0);
    static constexpr const auto Blue = Color(0, 0, 1);
    static constexpr const auto Yellow = Color(1.0f, 1.0f, 0.0f);
    static constexpr const auto Cyan = Color(0.0f, 1.0f, 1.0f);
    static constexpr const auto Magenta = Color(1.0f, 0.0f, 1.0f);

    static constexpr const auto Wine = Color(0.5f, 0.0f, 0.0f);
    static constexpr const auto Forest = Color(0.0f, 0.5f, 0.0f);
    static constexpr const auto Marine = Color(0.0f, 0.0f, 0.5f);

    // Others
    static constexpr const auto NeonOrange = Color(1.0f, 0.6470588f, 0.0f);
    static constexpr const auto LightMagenta = Color(1.0f, 0.5f, 1.0f);
    static constexpr const auto BabyPink = Color(1.0f, 0.70588235294f, 1.0f);
    static constexpr const auto Azure = Color(0.0f, 0.5f, 1.0f);
};
} // namespace Mana
