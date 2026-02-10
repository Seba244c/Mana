#include "Log.h"

#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Mana {
std::shared_ptr<spdlog::logger> Log::s_Logger;
std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
#ifdef M_DEBUG_INFO
std::shared_ptr<spdlog::logger> Log::s_DbgLogger;
#endif

void Log::Init() {
    // Logsinks
    std::vector<spdlog::sink_ptr> logSinks(2);

    std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> consoleSink =
        std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    consoleSink->set_pattern("%^[%T] [thread %t] (%n%s:%#)%$ %v");
    consoleSink->set_color(spdlog::level::info, "\033[42m\033[30m");
    consoleSink->set_color(spdlog::level::warn, "\033[43m\033[30m");
    consoleSink->set_color(spdlog::level::err, "\033[41m\033[30m");
    consoleSink->set_color(spdlog::level::critical,
                           "\033[4m\033[1m\033[41m\033[33m");
    logSinks[0] = std::move(consoleSink);

    // File
    logSinks[1] =
        std::make_shared<spdlog::sinks::basic_file_sink_mt>("Mana.log", true);
    logSinks[1]->set_pattern("%l: [%T] [thread %t] (%n%s:%#) %v");

    // Loggers
    s_Logger = std::make_shared<spdlog::logger>("App/", begin(logSinks),
                                                end(logSinks));
    s_Logger->set_level(spdlog::level::trace);
    s_Logger->flush_on(spdlog::level::trace);

    s_CoreLogger = std::make_shared<spdlog::logger>("Mana/", begin(logSinks),
                                                    end(logSinks));
    s_CoreLogger->set_level(spdlog::level::trace);
    s_CoreLogger->flush_on(spdlog::level::trace);

#ifdef M_DEBUG_INFO
    // Dbg logger!
    std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> dbgSink =
        std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    dbgSink->set_pattern("%^[%T] (%s:%#)%$ %v");
    dbgSink->set_color(spdlog::level::trace, "\033[44m\033[30m");
    s_DbgLogger = std::make_shared<spdlog::logger>("dbg", dbgSink);
    s_DbgLogger->set_level(spdlog::level::trace);
    s_DbgLogger->flush_on(spdlog::level::trace);
#endif
}
} // namespace Mana
