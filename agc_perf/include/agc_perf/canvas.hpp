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

#include "agc_perf/a2.hpp"
#include "agc_perf/canvas_fwd.hpp"

#include <cstdint>
#include <ostream>
#include <vector>


namespace agc_perf {

struct Canvas
{
    A2<uint32_t> size;
    std::vector<double> values;
    auto operator==(const Canvas&) const noexcept -> bool = default;
};

auto operator<<(std::ostream& s, const Canvas& canvas)
    -> std::ostream&;

} // namespace agc_perf
