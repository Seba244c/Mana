#pragma once
#include <mpch.h>

#include "Mana/Graphics/GraphicsContext.h"
#include "Mana/IO/Window.h"

namespace Mana {
class Platform {
  public:
    static void Init();
    static Scope<Window> CreateWindow(WindowSpecs &specs);
    static Scope<GraphicsContext> CreateGraphicsContext(Window &window);
};

class Time {
  public:
    typedef std::chrono::duration<double, std::milli> Duration;
    typedef std::chrono::time_point<std::chrono::steady_clock, Duration> Moment;

    [[nodiscard]] static Moment Now();
    [[nodiscard]] static Duration Elapsed();
    [[nodiscard]] static Duration Delta();
    [[nodiscard]] static double FPS();
    [[nodiscard]] static Duration Millis(double value);
    [[nodiscard]] static Duration Seconds(double value);

    static void CountFrame();
};
} // namespace Mana
