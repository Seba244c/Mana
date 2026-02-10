#pragma once
#include <mpch.h>

#include <spdlog/spdlog.h>

namespace Mana {
class Log {
  public:
    static void Init();

    static Ref<spdlog::logger> &GetLogger() { return s_Logger; }
    static Ref<spdlog::logger> &GetCoreLogger() { return s_CoreLogger; }

  private:
    static Ref<spdlog::logger> s_Logger;
    static Ref<spdlog::logger> s_CoreLogger;

#ifdef M_DEBUG_INFO
  public:
    static Ref<spdlog::logger> &GetDbgLogger() { return s_DbgLogger; }

  private:
    static Ref<spdlog::logger> s_DbgLogger;
#endif
};
} // namespace Mana

#ifdef M_DEBUG_INFO
#define M_TRACE(...) SPDLOG_LOGGER_TRACE(::Mana::Log::GetLogger(), __VA_ARGS__)
#else
#define M_TRACE(...)
#endif
#define M_INFO(...) SPDLOG_LOGGER_INFO(::Mana::Log::GetLogger(), __VA_ARGS__)
#define M_WARN(...) SPDLOG_LOGGER_WARN(::Mana::Log::GetLogger(), __VA_ARGS__)
#define M_ERROR(...) SPDLOG_LOGGER_ERROR(::Mana::Log::GetLogger(), __VA_ARGS__)
#define M_CRITICAL(...)                                                        \
    SPDLOG_LOGGER_CRITICAL(::Mana::Log::GetLogger(), __VA_ARGS__)

#ifdef M_DEBUG_INFO
#define MC_TRACE(...)                                                          \
    SPDLOG_LOGGER_TRACE(::Mana::Log::GetCoreLogger(), __VA_ARGS__)
#else
#define MC_TRACE(...)
#endif
#define MC_INFO(...)                                                           \
    SPDLOG_LOGGER_INFO(::Mana::Log::GetCoreLogger(), __VA_ARGS__)
#define MC_WARN(...)                                                           \
    SPDLOG_LOGGER_WARN(::Mana::Log::GetCoreLogger(), __VA_ARGS__)
#define MC_ERROR(...)                                                          \
    SPDLOG_LOGGER_ERROR(::Mana::Log::GetCoreLogger(), __VA_ARGS__)
#define MC_CRITICAL(...)                                                       \
    SPDLOG_LOGGER_CRITICAL(::Mana::Log::GetCoreLogger(), __VA_ARGS__)

#ifdef M_DEBUG_INFO
#define dbg(x)                                                                 \
    SPDLOG_LOGGER_TRACE(::Mana::Log::GetDbgLogger(), "{} = {}", #x, x);
#else
#define dbg(x)
#endif
