#pragma once

#include "agc_rt/types/point_2d.hpp"


namespace agc_rt {

inline auto mandelbrot_func(const Point2d_d& c, const Point2d_d& z) noexcept
    -> Point2d_d
{
    // Complex z * z + c;
    return { z[0]*z[0] - z[1]*z[1] + c[0],
             2*z[0]*z[1] + c[1] };
}

} // namespace agc_rt
