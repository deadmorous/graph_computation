/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "agc_app/types/range.hpp"

#include <cstdint>


namespace agc_app {

struct Grid2dSpec final
{
    std::array<Range<double>, 2> rect;
    std::array<double, 2> resolution;
};

GCLIB_STRUCT_TYPE(Grid2dSpec, rect, resolution);

} // namespace agc_app
