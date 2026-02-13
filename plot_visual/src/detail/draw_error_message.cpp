/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#include "plot_visual/detail/draw_error_message.hpp"

#include <QPainter>


namespace plot::detail {

auto draw_error_message(const QRect& rect,
                        QPainter& painter,
                        const char* message) -> void
{
    painter.fillRect(rect, Qt::white);
    painter.setPen(Qt::red);
    painter.drawText(
        rect, Qt::AlignHCenter | Qt::AlignVCenter, QString::fromUtf8(message));
}

} // namespace plot::detail
