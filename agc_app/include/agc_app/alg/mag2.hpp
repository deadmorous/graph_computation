#pragma once

#include "agc_app/types/point_2d.hpp"


namespace agc_app {

inline auto mag2(const Point2d_d& v) noexcept
    -> double
{ return v[0]*v[0] + v[1]*v[1]; }

} // namespace agc_app
