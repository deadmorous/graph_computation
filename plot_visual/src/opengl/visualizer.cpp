/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#include "plot_visual/opengl/visualizer.hpp"

#include <QPainter>

#include <cassert>

namespace plot {

namespace {
} // anonymous namespace

class OpenGLVisualizer::Impl final
{
public:
    auto bind_data(const gc::Value* data) -> void;
    auto bind_opengl_widget(QOpenGLWidget& widget,
                            QOpenGLFunctions_3_3_Core& ogl) -> void;
    auto paint_3d(const QRect& rect) -> void;
    auto paint_2d(const QRect& rect, QPainter&) -> void;
};

OpenGLVisualizer::OpenGLVisualizer() :
    impl_{ std::make_unique<Impl>() }
{}

OpenGLVisualizer::~OpenGLVisualizer() = default;

auto OpenGLVisualizer::bind_data(const gc::Value* data) -> void
{ impl_->bind_data(data); }

auto OpenGLVisualizer::bind_opengl_widget(
    QOpenGLWidget& widget, QOpenGLFunctions_3_3_Core& ogl) -> void
{ impl_->bind_opengl_widget(widget, ogl); }

auto OpenGLVisualizer::paint_3d(const QRect& rect) -> void
{ impl_->paint_3d(rect); }

auto OpenGLVisualizer::paint_2d(const QRect& rect, QPainter& painter) -> void
{ impl_->paint_2d(rect, painter); }

} // plot
