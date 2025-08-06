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

#include "agc_rt/types/point_2d.hpp"


namespace agc_rt {

inline auto mag2(const Point2d_d& v) noexcept
    -> double
{ return v[0]*v[0] + v[1]*v[1]; }

} // namespace agc_rt
