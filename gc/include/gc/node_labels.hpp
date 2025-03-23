/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#pragma once

#include "gc/node_index.hpp"

#include "common/strong_span.hpp"

#include <string_view>


namespace gc {

using NodeLabels = common::StrongSpan<std::string_view, gc::NodeIndex>;

} // namespace gc
