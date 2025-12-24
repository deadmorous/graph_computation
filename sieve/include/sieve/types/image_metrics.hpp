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

#include "common/struct_type_macro.hpp"

#include <iostream>
#include <vector>

namespace sieve {

struct ImageMetrics
{
    std::vector<double> histogram;
    std::vector<double> edge_histogram;
};
GCLIB_STRUCT_TYPE(ImageMetrics, histogram, edge_histogram);

auto operator<<(std::ostream&, const ImageMetrics&) -> std::ostream&;

} // namespace sieve
