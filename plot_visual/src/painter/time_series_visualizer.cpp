/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#include "plot_visual/painter/time_series_visualizer.hpp"

#include "gc_types/live_time_series.hpp"

#include "plot_visual/axis.hpp"
#include "plot_visual/axes_2d.hpp"
#include "plot_visual/color.hpp"
#include "plot_visual/linear_coordinate_mapping.hpp"
#include "plot_visual/layout.hpp"

#include <QPainterPath>

namespace plot {

using namespace gc_types;


TimeSeriesVisualizer::TimeSeriesVisualizer(LiveTimeSeries& time_series,
                                           Attributes& attributes) :
    time_series_{&time_series},
    attributes_{&attributes}
{}

auto TimeSeriesVisualizer::paint(const QRect& rect, QPainter& painter) -> void
{
    auto frames = time_series_->frames();
    if (frames.empty())
        return;

    using CoordMap = plot::LinearCoordinateMapping<double, int>;
    using Axis = plot::Axis<CoordMap>;
    using Axes = plot::Axes2d<Axis, Axis>;

    auto frame_capacity = time_series_->frame_capacity();
    int frame_count = frames.size();    // NOTE: Signedness is important
    auto state_count = frames.front().values.size();

    auto y_max = time_series_->value_range().end;

    auto axes_painter = plot::AxesPainter{
        Axes{
            .x = {
                .mapping{
                    .from{
                        .begin = -static_cast<double>(frame_capacity),
                        .end = 0}},
                .label = attributes_->x_label },
            .y = {
                .mapping{ .from = { .begin = 0., .end = y_max }},
                .label = attributes_->y_label },
            .title = attributes_->title
        },
        rect
    };
    const auto& axes = axes_painter.axes();

    [[maybe_unused]]
    auto rc = axes_painter.layout().rect(plot::layout::central);

    painter.setRenderHint(QPainter::Antialiasing);

    for (size_t state=0; state<state_count; ++state)
    {
        auto path = QPainterPath{};
        auto path_started = false;
        for (int frame_index=0; frame_index<frame_count; ++frame_index)
        {
            const auto& frame = frames[frame_index];
            if (frame.values.size() <= state)
                continue;
            auto px = axes.x.mapping(frame_index - frame_count);
            auto py = axes.y.mapping(frame.values[state]);
            if (path_started)
                path.lineTo(px, py);
            else
            {
                path.moveTo(px, py);
                path_started = true;
            }
        }
        if (!path.isEmpty())
        {
            constexpr int pen_width = 2;
            auto pen_color = qcolor(map_color(attributes_->palette, state));
            auto pen_style = Qt::SolidLine;
            auto lightness = pen_color.lightnessF();
            constexpr auto lightness_threshold = 0.9;   // TODO better
            if (lightness > lightness_threshold)
            {
                lightness /= 2;
                pen_style = Qt::DashLine;
                pen_color = QColor::fromHslF(
                    pen_color.hslHue(), pen_color.hslSaturationF(), lightness);
            }
            auto pen =
                QPen(pen_color, pen_width, pen_style);
            painter.strokePath(path, pen);
        }
    }

    axes_painter.draw(painter);
}

} // namespace plot
