#pragma once

#include "gc/struct_type_macro.hpp"

#include <cstdint>


namespace agc_app {

struct LinSpaceSpec final
{
    double      first{0};
    double      last{1};
    uint32_t    count{11};
};

GCLIB_STRUCT_TYPE(LinSpaceSpec, first, last, count);

} // namespace agc_app
