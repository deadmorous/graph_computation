/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#pragma once

#include <QRect>

class QPainter;

namespace plot::detail {

auto draw_error_message(const QRect&, QPainter&, const char*) -> void;

} // namespace plot::detail
