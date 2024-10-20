#pragma once

#include <array>
#include <cstdint>

namespace gc_app {

// TODO: Make these strong types
using ColorComponent = uint8_t;
using Color = uint32_t;

inline constexpr auto rgba(ColorComponent r,
                           ColorComponent g,
                           ColorComponent b,
                           ColorComponent a = 0xff)
    noexcept -> Color
{
    return
          (Color{a} << 24)
        | (Color{r} << 16)
        | (Color{g} <<  8)
        | (Color{b});
}

inline constexpr auto rgba(Color rgb, ColorComponent a = 0xff) noexcept
    -> Color
{ return (Color{a} << 24) | (rgb & 0xffffff); }

inline constexpr auto r_g_b_a(Color rgba) noexcept
    -> std::array<ColorComponent, 4>
{
    return {
        static_cast<ColorComponent>((rgba >> 16) & 0xff),
        static_cast<ColorComponent>((rgba >> 8) & 0xff),
        static_cast<ColorComponent>(rgba & 0xff ),
        static_cast<ColorComponent>((rgba >> 24) & 0xff)
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
{ return rgba(0, 0); }

inline auto black_color()
{ return rgba(0, 0xff); }

inline auto white_color()
{ return rgba(0xffffff, 0xff); }

} // namespace gc_app
