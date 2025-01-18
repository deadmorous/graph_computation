#pragma once

#include "common/strong.hpp"

#include <cstdint>


namespace gc {

GCLIB_STRONG_TYPE(Count, uint32_t, 0, common::StrongCountFeatures);
GCLIB_STRONG_TYPE(Index, uint32_t, 0, common::StrongIndexFeatures<Count>);

} // namespace gc
