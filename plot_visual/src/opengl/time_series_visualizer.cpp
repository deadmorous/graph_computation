/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#include "plot_visual/opengl/time_series_visualizer.hpp"

#include "gc_types/live_time_series.hpp"

#include "plot_visual/axis.hpp"
#include "plot_visual/axes_2d.hpp"
#include "plot_visual/color.hpp"
#include "plot_visual/layout.hpp"
#include "plot_visual/linear_coordinate_mapping.hpp"

namespace plot::opengl {

using namespace gc_types;

namespace {

} // anonymous namespace

struct TimeSeriesVisualizer::Storage
{
    Storage(LiveTimeSeries& time_series, Attributes& attributes):
        time_series{time_series},
        attributes{attributes}
    {
        time_series.register_checkpoint(checkpoint);
    }

    LiveTimeSeries& time_series;
    const Attributes& attributes;
    LiveTimeSeries::Checkpoint checkpoint;
};

TimeSeriesVisualizer::~TimeSeriesVisualizer() = default;

TimeSeriesVisualizer::TimeSeriesVisualizer(LiveTimeSeries& time_series,
                                           Attributes& attributes) :
    storage_{std::make_unique<Storage>(time_series, attributes) }
{}

auto TimeSeriesVisualizer::bind_data(const gc::Value* data) -> void
{
    std::ignore = data;
}

auto TimeSeriesVisualizer::bind_opengl_widget(QOpenGLWidget&, OpenGLFunctions&)
    -> void
{}

auto TimeSeriesVisualizer::paint_3d(const QRect&) -> void
{}

auto TimeSeriesVisualizer::paint_2d(const QRect&, QPainter&) -> void
{}


} // namespace plot::opengl
