/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#include "plot_visual/painter/visualizer.hpp"

#include "plot_visual/detail/draw_error_message.hpp"

#include "common/defer.hpp"

#include <QPainter>

#include <cassert>

namespace plot::painter {

auto safe_paint(Visualizer* visualizer,
                const QRect& rect,
                QPainter& painter) -> void
{
    painter.save();
    auto restore_painter = common::Defer{ [&]{ painter.restore(); } };

    if (visualizer)
    {
        try
        {
            visualizer->paint(rect, painter);
        }
        catch(std::exception& e)
        {
            detail::draw_error_message(rect, painter, e.what());
        }
    }
    else
        detail::draw_error_message(rect, painter, "No visualizer");
}

} // plot::painter
