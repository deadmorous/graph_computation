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

#include <memory>

class QOpenGLWidget;
class QOpenGLFunctions_3_3_Core;
class QPainter;

namespace plot {

class OpenGLVisualizer final
{
public:
    OpenGLVisualizer();
    ~OpenGLVisualizer();

    auto bind_data(const gc::Value* data) -> void;
    auto bind_opengl_widget(QOpenGLWidget&, QOpenGLFunctions_3_3_Core&) -> void;
    auto paint_3d(const QRect& rect) -> void;
    auto paint_2d(const QRect& rect, QPainter&) -> void;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace plot
