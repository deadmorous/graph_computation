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

namespace plot::painter {

class Visualizer
{
public:
    virtual ~Visualizer() = default;

    virtual auto paint(const QRect&, QPainter&) -> void = 0;
};

auto safe_paint(Visualizer*, const QRect&, QPainter&) -> void;

} // namespace plot::painter
