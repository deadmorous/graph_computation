/** @file
 * @brief Compatibility shim — delegates to mpk/mix/macro.hpp.
 *
 * New code should include <mpk/mix/macro.hpp> and use the MPKMIX_ prefix
 * directly.  This header exists only to keep existing graph_computation
 * sources building without change.
 *
 * Copyright (C) 2024 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "mpk/mix/macro.hpp"

// ---------------------------------------------------------------------------
// Aliases: GCLIB_* → MPKMIX_*
// ---------------------------------------------------------------------------

// GCLIB_NUM_ARGS differs from the MPKMIX variant in that the historical gc
// implementation hardcoded the 'extra' sentinel internally, so callers wrote
// GCLIB_NUM_ARGS(a, b) rather than GCLIB_NUM_ARGS(extra, a, b).
// We preserve that calling convention here.
#define GCLIB_NUM_ARGS(...) MPKMIX_NUM_ARGS(extra, ##__VA_ARGS__)

#define GCLIB_STRINGIZE(x)   MPKMIX_STRINGIZE(x)
#define GCLIB_IDENTITY(x)    MPKMIX_IDENTITY(x)

#define GCLIB_COMMA_IMPL     ,
#define GCLIB_SEMICOLON_IMPL ;
#define GCLIB_COLON_IMPL     :

#define GCLIB_MAP_SEP_LIST(m, p, s, ...) \
    MPKMIX_MAP_SEP_LIST(m, p, s, ##__VA_ARGS__)
#define GCLIB_MAP_COMMA_SEP_LIST(m, p, ...) \
    MPKMIX_MAP_COMMA_SEP_LIST(m, p, ##__VA_ARGS__)

#define GCLIB_DEFAULT_A0_TO(D, ...) MPKMIX_DEFAULT_A0_TO(D, ##__VA_ARGS__)
#define GCLIB_DEFAULT_A1_TO(D, ...) MPKMIX_DEFAULT_A1_TO(D, ##__VA_ARGS__)
