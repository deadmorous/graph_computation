/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024-2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "gc/weak_node_index.hpp"

#include "mpk/mix/strong/strong.hpp"

#include <cassert>
#include <cstdint>
#include <limits>


namespace gc {

MPKMIX_STRONG_TYPE(NodeCount, WeakNodeIndex, mpk::mix::StrongCountFeatures);

MPKMIX_STRONG_TYPE(
    NodeIndex, WeakNodeIndex, mpk::mix::StrongIndexFeatures<NodeCount>);

namespace literals {
MPKMIX_STRONG_LITERAL_SUFFIX(NodeCount, _gc_nc);
MPKMIX_STRONG_LITERAL_SUFFIX(NodeIndex, _gc_n);
} // namespace literals

} // namespace gc
