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

#include "common/defer.hpp"

#include <QPainter>

#include <cassert>

namespace plot {

namespace {

auto draw_error_message(const QRect& rect,
                        QPainter& painter,
                        const char* message) -> void
{
    painter.fillRect(rect, Qt::white);
    painter.setPen(Qt::red);
    painter.drawText(
        rect, Qt::AlignHCenter | Qt::AlignVCenter, QString::fromUtf8(message));
}

} // anonymous namespace

auto safe_paint(PainterVisualizer* visualizer,
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
            draw_error_message(rect, painter, e.what());
        }
    }
    else
        draw_error_message(rect, painter, "No visualizer");
}

} // plot
