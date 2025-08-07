#pragma once

#include "agc_app_rt/types/point_2d.hpp"


namespace agc_app_rt {

inline auto mag2(const Point2d_d& v) noexcept
    -> double
{ return v[0]*v[0] + v[1]*v[1]; }

} // namespace agc_app_rt
