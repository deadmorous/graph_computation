#pragma once

#include "gc/weak_node_index.hpp"

#include "common/strong.hpp"

#include <cassert>
#include <cstdint>
#include <limits>


namespace gc {

GCLIB_STRONG_TYPE(NodeCount, WeakNodeIndex, 0, common::StrongCountFeatures);

GCLIB_STRONG_TYPE(
    NodeIndex, WeakNodeIndex, 0, common::StrongIndexFeatures<NodeCount>);

namespace literals {
GCLIB_STRONG_LITERAL_SUFFIX(NodeCount, _gc_nc);
GCLIB_STRONG_LITERAL_SUFFIX(NodeIndex, _gc_n);
} // namespace literals

} // namespace gc
