#pragma once

#include "common/struct_type_macro.hpp"

#include <cstdint>


namespace agc_rt {

struct LinSpaceSpec final
{
    double      first{0};
    double      last{1};
    uint32_t    count{11};
};

GCLIB_STRUCT_TYPE(LinSpaceSpec, first, last, count);

} // namespace agc_rt
