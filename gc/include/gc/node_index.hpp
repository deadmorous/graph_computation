#pragma once

#include "common/strong.hpp"

#include <cassert>
#include <cstdint>
#include <limits>


namespace gc {

GCLIB_STRONG_TYPE(NodeCount, uint32_t, 0, common::StrongCountFeatures);

GCLIB_STRONG_TYPE(
    NodeIndex, uint32_t, 0, common::StrongIndexFeatures<NodeCount>);

namespace literals {
GCLIB_STRONG_LITERAL_SUFFIX(NodeCount, _gc_nc);
GCLIB_STRONG_LITERAL_SUFFIX(NodeIndex, _gc_n);
} // namespace literals

} // namespace gc
