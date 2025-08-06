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
