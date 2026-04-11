/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#pragma once

// This "logging engine" just disables all logging, so that the arguments to
// logging macros are not even evaluated.
#define GC_LOG_NONE 0

// Normally we should use the Quill logging engine
#define GC_LOG_QUILL 1



#ifndef GRAPH_COMPUTATION_LOG_ENGINE

// Use Quill for logging by default;
// to override, pass -DGRAPH_COMPUTATION_LOG_ENGINE=... to cmake
#define GRAPH_COMPUTATION_LOG_ENGINE GC_LOG_QUILL

#endif // !GRAPH_COMPUTATION_LOG_ENGINE
