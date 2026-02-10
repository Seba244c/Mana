#ifndef H_MANAPCH
#define H_MANAPCH

#ifdef MANAPL_WINDOWS
#ifndef NOMINMAX
// See
// github.com/skypjack/entt/wiki/Frequently-Asked-Questions#warning-c4003-the-min-the-max-and-the-macro
#define NOMINMAX
#endif

// Inlcude windows
#endif

// clang-format off
#include <sstream>
#include <string>
#include <string_view>

#include <map>
#include <queue>
#include <set>
#include <span>
#include <vector>

#include <cstdint>
#include <functional>
#include <mutex>

#include <memory>
#include <filesystem>
#include <fstream>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/norm.hpp>

#include <flecs.h>

#ifdef MANAB_OPENGL
#include "Mana/Graphics/Backends/OpenGL/GL.h"
#endif
#ifdef MANAB_GLFW
#include "GLFW/glfw3.h"
#endif
#ifdef MANAB_SDL
#include <SDL3/SDL.h>
#endif

// TODO MANAB_MINIAUDIO
#include "miniaudio.h"

#ifdef M_DEBUG
#define ISDEBUG true
#else
#define ISDEBUG false
#endif

#ifdef M_HEADLESS
#define ISHEADLESS true
#else
#define ISHEADLESS false
#endif

#include "Mana/Core/Core.h"

// clang-format on
#endif // !H_MANAPCH
