/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "gc_app/palette.hpp"


namespace gc_app {

auto map_color(const IndexedPalette& palette, uint32_t value)
    -> Color
{
    if (value >= palette.color_map.size())
        return palette.overflow_color;

    return palette.color_map[value];
}

auto average_color(const IndexedPalette& palette, bool with_overflow)
    -> Color
{
    auto avg_rgba = std::array{0u, 0u, 0u, 0u};
    size_t count = 0;

    auto add_color = [&](Color color)
    {
        auto components = r_g_b_a(color);
        for (size_t i=0; i<4; ++i)
            avg_rgba[i] += components[i].v;
        ++count;
    };

    for (auto color: palette.color_map)
        add_color(color);

    if (with_overflow)
        add_color(palette.overflow_color);

    if (count == 0)
        return transparent_color();

    return rgba(ColorComponent(avg_rgba[0]/count),
                ColorComponent(avg_rgba[1]/count),
                ColorComponent(avg_rgba[2]/count),
                ColorComponent(avg_rgba[3]/count));
}

} // namespace gc_app
