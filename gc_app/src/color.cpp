/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "gc_app/color.hpp"

#include <algorithm>
#include <cmath>


namespace gc_app {

namespace {

inline auto clamp(double p)
    -> double
{ return std::min(1., std::max(0., p)); }

auto interp_color_impl(Color color0, Color color1, double p)
    -> Color
{
    auto interp = [&](ColorComponent component0,
                      ColorComponent component1) -> ColorComponent
    {
        return static_cast<ColorComponent>(
            std::lround(component0.v*(1-p) + component1.v*p));
    };
    auto c0 = r_g_b_a(color0);
    auto c1 = r_g_b_a(color1);
    return rgba(interp(c0[0], c1[0]),
                interp(c0[1], c1[1]),
                interp(c0[2], c1[2]),
                interp(c0[3], c1[3]));
}

} // anonymous namespace

auto contrast_color(Color color)
    -> Color
{
    auto components = r_g_b_a(color);
    constexpr uint32_t threshold = 3 * 0x7f;
    auto n =
        static_cast<Color::Weak>(components[0].v) +
        static_cast<Color::Weak>(components[1].v) +
        static_cast<Color::Weak>(components[2].v);
    return n > threshold ? black_color() : white_color();
}

auto interp_color(Color color0, Color color1, double p)
    -> Color
{ return interp_color_impl(color0, color1, clamp(p)); }

auto interp_color(Color color0, Color color1, double p, double p_pow)
    -> Color
{ return interp_color_impl(color0, color1, pow(clamp(p), p_pow)); }

auto threshold_color(Color color0,
                     Color color1,
                     double p,
                     double threshold)
    -> Color
{ return p < threshold ? color0 : color1; }

} // namespace gc_app
