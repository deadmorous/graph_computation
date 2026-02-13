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

#include "plot_visual/opengl/visualizer.hpp"

#include "gc_types/live_time_series_fwd.hpp"
#include "gc_types/palette.hpp"

namespace plot::opengl {

class TimeSeriesVisualizer final : public Visualizer
{
public:
    struct Attributes final
    {
        gc_types::IndexedPalette palette;
        QString x_label;
        QString y_label;
        QString title;
    };

    ~TimeSeriesVisualizer();

    // NOTE
    // Accept non-const LiveTimeSeries& because of checkpointing
    // Accept non-const Attributes& to avoid passing a temporary
    TimeSeriesVisualizer(gc_types::LiveTimeSeries&, Attributes& attributes);

    auto bind_data(const gc::Value* data) -> void override;
    auto bind_opengl_widget(QOpenGLWidget&, OpenGLFunctions&) -> void override;
    auto paint_3d(const QRect&) -> void override;
    auto paint_2d(const QRect&, QPainter&) -> void override;

    struct Storage;

private:
    std::unique_ptr<Storage> storage_;
};

} // namespace plot::opengl
