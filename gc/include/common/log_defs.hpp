// Compatibility shim — use mpk/mix/log/defs.hpp directly in new code.
#pragma once

// Propagate GRAPH_COMPUTATION_LOG_ENGINE to MPKMIX_LOG_ENGINE.
#ifdef GRAPH_COMPUTATION_LOG_ENGINE
#define MPKMIX_LOG_ENGINE GRAPH_COMPUTATION_LOG_ENGINE
#endif

#include "mpk/mix/log/defs.hpp"

#define GC_LOG_NONE MPKMIX_LOG_NONE
#define GC_LOG_QUILL MPKMIX_LOG_QUILL
