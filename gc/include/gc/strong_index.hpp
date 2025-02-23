#pragma once

#include "common/strong.hpp"

#include <cstdint>


namespace gc {

GCLIB_STRONG_TYPE(Count, uint32_t, common::StrongCountFeatures);
GCLIB_STRONG_TYPE(Index, uint32_t, common::StrongIndexFeatures<Count>);

} // namespace gc
