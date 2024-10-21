#pragma once

#include "common/strong.hpp"

#include <array>
#include <cstdint>


namespace gc_app {

GCLIB_STRONG_TYPE(ColorComponent, uint8_t);

GCLIB_STRONG_TYPE(Color, uint32_t);

inline constexpr auto rgba(ColorComponent r,
                           ColorComponent g,
                           ColorComponent b,
                           ColorComponent a = ColorComponent{0xff})
    noexcept -> Color
{
    return Color{
          (Color::Weak{a.v} << 24)
        | (Color::Weak{r.v} << 16)
        | (Color::Weak{g.v} <<  8)
        | (Color::Weak{b.v}) };
}

inline constexpr auto rgba(Color rgb,
                           ColorComponent a = ColorComponent{0xff}) noexcept
    -> Color
{ return Color{(Color{a.v}.v << 24) | (rgb.v & 0xffffff)}; }

inline constexpr auto r_g_b_a(Color rgba) noexcept
    -> std::array<ColorComponent, 4>
{
    return {
        static_cast<ColorComponent>((rgba.v >> 16) & 0xff),
        static_cast<ColorComponent>((rgba.v >> 8) & 0xff),
        static_cast<ColorComponent>(rgba.v & 0xff ),
        static_cast<ColorComponent>((rgba.v >> 24) & 0xff)
    };
}

auto contrast_color(Color color)
    -> Color;

auto interp_color(Color color0, Color color1, double p)
    -> Color;

auto interp_color(Color color0, Color color1, double p, double p_pow)
    -> Color;

auto threshold_color(Color color0,
                     Color color1,
                     double p,
                     double threshold = 0.5)
    -> Color;

inline auto transparent_color()
{ return rgba(Color{0}, ColorComponent{0}); }

inline auto black_color()
{ return rgba(Color{0}, ColorComponent{0xff}); }

inline auto white_color()
{ return rgba(Color{0xffffff}, ColorComponent{0xff}); }

} // namespace gc_app
