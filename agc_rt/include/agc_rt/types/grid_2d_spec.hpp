#pragma once

#include "agc_rt/types/range.hpp"

#include <cstdint>


namespace agc_rt {

struct Grid2dSpec final
{
    std::array<Range<double>, 2> rect;
    std::array<double, 2> resolution;
};

GCLIB_STRUCT_TYPE(Grid2dSpec, rect, resolution);

} // namespace agc_rt
