/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#include "plot_visual/painter/time_series_histogram_visualizer.hpp"

#include "gc_types/live_time_series.hpp"

#include "plot_visual/axis.hpp"
#include "plot_visual/axes_2d.hpp"
#include "plot_visual/color.hpp"
#include "plot_visual/layout.hpp"
#include "plot_visual/linear_coordinate_mapping.hpp"

namespace plot {

using namespace gc_types;


TimeSeriesHistogramVisualizer::TimeSeriesHistogramVisualizer(
        LiveTimeSeries& time_series,
        Attributes& attributes) :
    time_series_{&time_series},
    attributes_{&attributes}
{}

auto TimeSeriesHistogramVisualizer::paint(
    const QRect& rect, QPainter& painter) -> void
{
    auto frames = time_series_->frames();
    if (frames.empty())
        return;

    using CoordMap = plot::LinearCoordinateMapping<double, int>;
    using Axis = plot::Axis<CoordMap>;
    using Axes = plot::Axes2d<Axis, Axis>;

    auto frame_capacity = time_series_->frame_capacity();
    int frame_count = frames.size();    // NOTE: Signedness is important

    auto axes_painter = plot::AxesPainter{
        Axes{
            .x = {
                .mapping{
                    .from{
                        .begin = -static_cast<double>(frame_capacity),
                        .end = 0}},
                .label = attributes_->x_label },
            .y = {
                .mapping{ .from = { .begin = 0., .end = 1. }},
                .label = attributes_->y_label },
            .title = attributes_->title
        },
        rect
    };
    const auto& axes = axes_painter.axes();

    auto rc = axes_painter.layout().rect(plot::layout::central);

    auto draw_metric = [&](int x, int index)
    {
        if (index >= frame_count)
            return;
        const auto& frame = frames[index];

        auto p0 = double{0};
        auto y0 = axes.y.mapping(p0);
        auto n = frame.values.size();
        for (size_t i=0; i<n; ++i)
        {
            auto p1 = p0 + frame.values[i];
            auto y1 = axes.y.mapping(p1);
            auto color = plot::qcolor(map_color(attributes_->palette, i));
            painter.fillRect(x, y0, 1, y1-y0, color);
            p0 = p1;
            y0 = y1;
        }
    };

    auto draw_interpolated_metric = [&](int x, double generation)
    {
        auto index_plus_t = frame_count - 1 + generation;
        if (index_plus_t < 0)
            return;

        auto i0 = static_cast<int>(index_plus_t);
        auto t = index_plus_t - i0;
        constexpr auto tol = 0.01;
        if (t < tol)
        {
            draw_metric(x, i0);
            return;
        }

        // TODO
        draw_metric(x, i0);
        return;
    };

    auto x_imap = axes.x.mapping.inverse();
    for (int x=rc.left(), right=rc.right(); x<right; ++x)
    {
        draw_interpolated_metric(x, x_imap(x));
    }

    axes_painter.draw(painter);
}

} // namespace plot
