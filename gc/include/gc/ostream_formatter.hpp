/** @file
 * @brief Compatibility shim — use mpk/mix/util/format_streamable.hpp directly in new code.
 *
 * Copyright (C) 2024-2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "mpk/mix/util/format_streamable.hpp"

namespace gc {

template <typename T>
using OstreamFormatter = mpk::mix::OstreamFormatter<T>;

} // namespace gc
