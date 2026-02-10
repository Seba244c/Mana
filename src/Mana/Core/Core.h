#pragma once

namespace Mana {
template <typename T> using Scope = std::unique_ptr<T>;

template <typename T, typename... Args>
constexpr Scope<T> CreateScope(Args &&...args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}

template <typename T> using Ref = std::shared_ptr<T>;
template <typename T, typename... Args>
constexpr Ref<T> CreateRef(Args &&...args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}

// Type list used in e.g. ECSReflect for component registration
template <typename... Ts> struct TypesList {};
} // namespace Mana

#define BIT(x) (1 << x)
#define M_BIND_EVENT_FN(fn)                                                    \
    [this](auto &&...args) -> decltype(auto) {                                 \
        return this->fn(std::forward<decltype(args)>(args)...);                \
    }
#define M_BIND_EVENT_STATIC_FN(fn)                                             \
    [](auto &&...args) -> decltype(auto) {                                     \
        return fn(std::forward<decltype(args)>(args)...);                      \
    }

#define EXPAND_MACRO(x) x
#define STRINGIFY_MACRO(x) #x
#define MACRO_TOSTRING(x) STRINGIFY_MACRO(x)
#include "Mana/Math/Color.h"

#include "Assert.h"
#include "Log.h"
#include "Utils.h"
