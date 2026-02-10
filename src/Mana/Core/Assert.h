#pragma once

#ifdef M_DEBUG
#if defined(MANAO_OSX)
#include <csignal>
#define M_DEBUGBREAK() raise(SIGTRAP)
#elif defined(MANAO_UNIX)
#include <csignal>
#define M_DEBUGBREAK() raise(SIGTRAP)
#else
#error "Platform doesn't support debugbreak yet!"
#endif

#define M_INTERNAL_ASSERT_IMPL(check, msg, ...)                                \
    {                                                                          \
        if (!(check)) {                                                        \
            MC_ERROR(msg, __VA_ARGS__);                                        \
            M_DEBUGBREAK();                                                    \
        }                                                                      \
    }

#define M_INTERNAL_ASSERT_WITH_MSG(check, ...)                                 \
    M_INTERNAL_ASSERT_IMPL(check, "Assertion failed: {0}", __VA_ARGS__)
#define M_INTERNAL_ASSERT_NO_MSG(check)                                        \
    M_INTERNAL_ASSERT_IMPL(                                                    \
        check, "Assertion '{0}' failed at {1}:{2}", STRINGIFY_MACRO(check),    \
        std::filesystem::path(__FILE__).filename().string(), __LINE__)

#define M_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
#define M_INTERNAL_ASSERT_GET_MACRO(...)                                       \
    EXPAND_MACRO(M_INTERNAL_ASSERT_GET_MACRO_NAME(                             \
        __VA_ARGS__, M_INTERNAL_ASSERT_WITH_MSG, M_INTERNAL_ASSERT_NO_MSG))

#define M_ASSERT(...)                                                          \
    EXPAND_MACRO(M_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(__VA_ARGS__))
#else
#define M_ASSERT(...)
#endif
