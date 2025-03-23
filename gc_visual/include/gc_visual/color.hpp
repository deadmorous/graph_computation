/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "gc_app/color.hpp"

#include <QColor>


namespace gc_visual {

inline auto qcolor(gc_app::Color color)
    -> QColor
{
    auto [r, g, b, a] = r_g_b_a(color);
    return { r.v, g.v, b.v, a.v };
}

inline auto color(QColor color)
    -> gc_app::Color
{
    return rgba(gc_app::ColorComponent(color.red()),
                gc_app::ColorComponent(color.green()),
                gc_app::ColorComponent(color.blue()),
                gc_app::ColorComponent(color.alpha()));
}

} // namespace gc_visual
