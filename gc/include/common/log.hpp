// Compatibility shim — use mpk/mix/log.hpp directly in new code.
#pragma once
#include "common/log_defs.hpp"
#include "mpk/mix/log.hpp"

namespace common {

using mpk::mix::LogLevel;
using mpk::mix::start_logging;
using mpk::mix::set_log_level;

// gc callers use quill_global_logger() as an implicit logger; delegate to mpk::mix.
inline auto quill_global_logger() -> mpk::mix::Logger*
{
    return mpk::mix::default_logger();
}

} // namespace common

#define GC_LOG_DEBUG(...) MPKMIX_LOG_DEBUG(::mpk::mix::default_logger(), ##__VA_ARGS__)
#define GC_LOG_INFO(...) MPKMIX_LOG_INFO(::mpk::mix::default_logger(), ##__VA_ARGS__)
#define GC_LOG_WARNING(...) MPKMIX_LOG_WARNING(::mpk::mix::default_logger(), ##__VA_ARGS__)
#define GC_LOG_ERROR(...) MPKMIX_LOG_ERROR(::mpk::mix::default_logger(), ##__VA_ARGS__)
#define GC_LOG_CRITICAL(...) MPKMIX_LOG_CRITICAL(::mpk::mix::default_logger(), ##__VA_ARGS__)
