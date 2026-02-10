#include "Platform.h"

#ifdef M_HEADLESS
#include "Mana/Backends/Headless/NoGraphics.h"
#include "Mana/IO/Backends/NoWindow.h"
#endif
#ifdef MANAB_OPENGL
#include "Mana/Graphics/Backends/OpenGL/GLContext.h"
#endif
#ifdef MANAB_SDL
#include "Mana/IO/Backends/SDLWindow.h"
#endif
#ifdef MANAB_GLFW
#include "Mana/IO/Backends/GLFWWindow.h"
#endif

namespace Mana {
// Time
static Time::Moment timeStart;

Time::Moment Time::Now() { return std::chrono::steady_clock::now(); };
Time::Duration Time::Elapsed() { return Now() - timeStart; }

static Time::Moment frameStart;
static double fpsDelta = 0;
static int fpsFrames;
static double fps;
static Time::Duration deltaTime;

void Time::CountFrame() {
    Time::Moment now = Now();
    deltaTime = now - frameStart;
    frameStart = now;

    fpsDelta += deltaTime.count();
    if (fpsDelta > 1000) {
        fpsDelta -= 1000;
        fps = fpsFrames;
        fpsFrames = 0;
    }

    fpsFrames++;
}

double Time::FPS() { return fps; }
Time::Duration Time::Delta() { return deltaTime; }

Time::Duration Time::Millis(double value) { return Duration(value); }
Time::Duration Time::Seconds(double value) { return Duration(1000 * value); }

void Platform::Init() {
    timeStart = Time::Now();
    frameStart = Time::Now();
}

Scope<Window> Platform::CreateWindow(WindowSpecs &specs) {
    Scope<Window> window = nullptr;
#ifdef M_HEADLESS
    if (!window) {
        MC_INFO("Choose window platform: Headless");
        window = CreateScope<NoWindow>();
    }
#else
    MC_TRACE("Headless platform not supported, skipping");
#endif
#ifdef MANAB_GLFW
    if (!window) {
        MC_INFO("Choose window platform: GLFW");
        window = CreateScope<GLFWWindow>(specs);
    } else
        MC_TRACE("Platform supported, but de-prioritized: GLFW");
#else
    MC_TRACE("GLFW not supported, skipping");
#endif
#ifdef MANAB_SDL
    if (!window) {
        MC_INFO("Choose window platform: SDL");
        window = CreateScope<SDLWindow>(specs);
    } else
        MC_TRACE("Platform supported, but de-prioritized: SDL");
#else
    MC_TRACE("SDL not supported, skipping");
#endif

    return window;
};
Scope<GraphicsContext> Platform::CreateGraphicsContext(Window &window) {
#ifdef M_HEADLESS
    return CreateScope<NoGraphics>();
#else
#ifdef MANAB_OPENGL
    return CreateScope<GLContext>(window);
#else
    return nullptr;
#endif
#endif
};

} // namespace Mana
