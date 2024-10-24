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

inline constexpr auto rgba(std::array<ColorComponent, 4> c) noexcept
    -> Color
{ return rgba(c[0], c[1], c[2], c[3]); }

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

inline constexpr auto transparent_color() noexcept
    -> Color
{ return rgba(Color{0}, ColorComponent{0}); }

inline constexpr auto black_color(ColorComponent a = {0xff}) noexcept
    -> Color
{ return rgba(Color{0}, a); }

inline constexpr auto white_color(ColorComponent a = {0xff}) noexcept
    -> Color
{ return rgba(Color{0xffffff}, a); }

inline constexpr auto gray_color(ColorComponent lightness,
                                 ColorComponent a = {0xff}) noexcept
    -> Color
{ return rgba(lightness, lightness, lightness, a); }

inline constexpr auto blend_colors(Color back, Color front) noexcept
    -> Color
{
    auto back_components = r_g_b_a(back);
    auto front_components = r_g_b_a(front);
    uint32_t ba = back_components[3].v;
    uint32_t fa = front_components[3].v;
    auto result_components = std::array<ColorComponent, 4>{};
    for (uint32_t i=0; i<4; ++i)
    {
        uint32_t b = back_components[i].v;
        uint32_t f = front_components[i].v;
        uint32_t c = (b*(0xff-fa) + f*fa)/0xff;
        result_components[i] = ColorComponent(c);
    }
    return rgba(result_components);
}

} // namespace gc_app
