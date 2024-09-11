#pragma once

#include "common/log_defs.hpp"


#if GRAPH_COMPUTATION_LOG_ENGINE==GC_LOG_NONE

#include <cstdint>

namespace common {

enum class LogLevel : uint8_t
{
    Debug,
    Info,
    Warning,
    Error,
    Critical,
    None
};

inline auto start_logging() -> void {}
inline auto set_log_level(LogLevel) -> void {}

} // namespace common

#define GC_LOG_DEBUG(...) static_assert(true)
#define GC_LOG_INFO(...) static_assert(true)
#define GC_LOG_WARNING(...) static_assert(true)
#define GC_LOG_ERROR(...) static_assert(true)
#define GC_LOG_CRITICAL(...) static_assert(true)



#elif GRAPH_COMPUTATION_LOG_ENGINE==GC_LOG_QUILL

#include "quill/Backend.h"
#include "quill/Frontend.h"
#include "quill/LogMacros.h"
#include "quill/Logger.h"
#include "quill/sinks/ConsoleSink.h"

namespace common {

using quill::LogLevel;

inline auto start_logging()
    -> void
{
    quill::BackendOptions backend_options;
    quill::Backend::start(backend_options);
}

inline auto quill_global_logger()
    -> quill::Logger*
{
    static quill::Logger* logger = nullptr;

    if (!logger)
    {
        auto console_sink =
            quill::Frontend::create_or_get_sink<quill::ConsoleSink>("sink_id_1");

        logger =
            quill::Frontend::create_or_get_logger("root", std::move(console_sink));
    }

    return logger;
}

inline auto set_log_level(LogLevel log_level)
    -> void
{

    quill_global_logger()->set_log_level(log_level);
}

} // namespace common

#define GC_LOG_DEBUG(...) \
    LOG_DEBUG(::common::quill_global_logger(), ##__VA_ARGS__)
#define GC_LOG_INFO(...) \
    LOG_INFO(::common::quill_global_logger(), ##__VA_ARGS__)
#define GC_LOG_WARNING(...) \
    LOG_WARNING(::common::quill_global_logger(), ##__VA_ARGS__)
#define GC_LOG_ERROR(...) \
    LOG_ERROR(::common::quill_global_logger(), ##__VA_ARGS__)
#define GC_LOG_CRITICAL(...) \
    LOG_CRITICAL(::common::quill_global_logger(), ##__VA_ARGS__)



#else
#error "Unknown logging engine " GRAPH_COMPUTATION_LOG_ENGINE
#endif
