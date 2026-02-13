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

#include "gc/value_fwd.hpp"

#include <QRect>

class QOpenGLWidget;
class QOpenGLFunctions_3_3_Core;
class QPainter;

namespace plot::opengl {

class Visualizer
{
public:
    using OpenGLFunctions = QOpenGLFunctions_3_3_Core;

    virtual ~Visualizer() = default;

    virtual auto bind_data(const gc::Value* data) -> void = 0;
    virtual auto bind_opengl_widget(QOpenGLWidget&,
                                    OpenGLFunctions&) -> void = 0;
    virtual auto paint_3d(const QRect& rect) -> void = 0;
    virtual auto paint_2d(const QRect& rect, QPainter&) -> void = 0;
};

auto safe_paint(Visualizer*, const QRect&, QPainter&) -> void;

} // namespace plot::opengl
