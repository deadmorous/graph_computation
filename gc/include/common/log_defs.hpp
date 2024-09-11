#pragma once

// This "logging engine" just disables all logging, so that the arguments to
// logging macros are not even evaluated.
#define GC_LOG_NONE 0

// Normally we should use the Quill logging engine
#define GC_LOG_QUILL 1



#ifndef GC_LOG_ENGINE

// Use Quill for logging by default;
// to override, specify -DGC_LOG_ENGINE=... in cmake
#define GRAPH_COMPUTATION_LOG_ENGINE GC_LOG_QUILL

#endif // !GC_LOG_ENGINE
