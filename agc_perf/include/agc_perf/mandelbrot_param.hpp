/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "agc_perf/rect.hpp"

#include <cstddef>


namespace agc_perf {

struct MandelbrotParam final
{
    Rect rect{
        Range{ -2.1, 0.7 },
        Range{ -1.2, 1.2 } };

    // A2<double> resolution{ 0.001, 0.002 };
    A2<double> resolution{ 0.01, 0.02 };

    size_t iter_count{100};

    double magnitude_threshold{ 1000 };
};

} // namespace agc_perf
