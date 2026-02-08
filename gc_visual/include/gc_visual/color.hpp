/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024-2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "gc_types/color.hpp"

#include <QColor>


namespace gc_visual {

inline auto qcolor(gc_types::Color color)
    -> QColor
{
    auto [r, g, b, a] = r_g_b_a(color);
    return { r.v, g.v, b.v, a.v };
}

inline auto color(QColor color)
    -> gc_types::Color
{
    return rgba(gc_types::ColorComponent(color.red()),
                gc_types::ColorComponent(color.green()),
                gc_types::ColorComponent(color.blue()),
                gc_types::ColorComponent(color.alpha()));
}

} // namespace gc_visual
