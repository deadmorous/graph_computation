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

#include <QPainter>
#include <QPixmap>

namespace plot {

using namespace gc_types;

struct TimeSeriesHistogramVisualizer::Storage
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

    QPixmap cache_image;
    double cache_end;
    bool cache_wrapped;
    static constexpr double cache_tol = 1e-5;
};

TimeSeriesHistogramVisualizer::~TimeSeriesHistogramVisualizer() = default;

TimeSeriesHistogramVisualizer::TimeSeriesHistogramVisualizer(
        LiveTimeSeries& time_series,
        Attributes& attributes) :
    storage_{std::make_unique<Storage>(time_series, attributes) }
{}

auto TimeSeriesHistogramVisualizer::paint(
    const QRect& rect, QPainter& painter) -> void
{
    auto& s = *storage_;

    auto frames = s.time_series.frames();
    if (frames.empty())
        return;

    using CoordMap = plot::LinearCoordinateMapping<double, int>;
    using Axis = plot::Axis<CoordMap>;
    using Axes = plot::Axes2d<Axis, Axis>;

    int frame_capacity = s.time_series.frame_capacity();
    int frame_count = frames.size();    // NOTE: Signedness is important

    auto axes_painter = plot::AxesPainter{
        Axes{
            .x = {
                .mapping{
                    .from{
                        .begin = -static_cast<double>(frame_capacity),
                        .end = 0}},
                .label = s.attributes.x_label },
            .y = {
                .mapping{ .from = { .begin = 0., .end = 1. }},
                .label = s.attributes.y_label },
            .title = s.attributes.title
        },
        rect
    };
    const auto& axes = axes_painter.axes();

    auto rc = axes_painter.layout().rect(plot::layout::central);

    auto frames_added = s.checkpoint.sync().frames_added;

    bool cache_valid =
        !s.cache_image.isNull() && s.cache_image.size() == rc.size();

    if (!cache_valid)
    {
        s.cache_image = QPixmap{ rc.size() };
        s.cache_image.fill();
        s.cache_end = 0;
        s.cache_wrapped = false;
    }

    auto cache_painter = QPainter{ &s.cache_image };

    auto draw_metric = [&](double width, int index)
    {
        if (index < 0 || index >= frame_count)
            return;
        const auto& frame = frames[index];

        auto p0 = double{0};
        auto y0 = axes.y.mapping(p0) - rc.top();
        auto n = frame.values.size();
        auto x = s.cache_end;
        if (x + s.cache_tol > rc.width())
        {
            x = 0;
            s.cache_wrapped = true;
        }
        s.cache_end = x + width;

        auto xi = static_cast<int>(std::floor(x));
        auto wi = static_cast<int>(std::ceil(width));
        for (size_t i=0; i<n; ++i)
        {
            auto p1 = p0 + frame.values[i];
            auto y1 = axes.y.mapping(p1) - rc.top();;
            auto color = plot::qcolor(map_color(s.attributes.palette, i));
            cache_painter.fillRect(xi, y0, wi, y1-y0, color);
            p0 = p1;
            y0 = y1;
        }
    };

    int first_index = cache_valid && frames_added.has_value()
        ? frame_count - static_cast<int>(frames_added.value())
        : 0;

    auto width = static_cast<double>(axes.x.mapping.to.length()) /
                 axes.x.mapping.from.length();
    if (frame_capacity <= rc.width())
    {
        for (int index=first_index; index<frame_count; ++index)
            draw_metric(width, index);
    }
    else
    {
        auto x_imap = axes.x.mapping.inverse();
        auto first_generation = first_index - frame_count;
        auto x = std::lround(axes.x.mapping(first_generation));
        for (auto right=rc.right(); x<right; ++x)
        {
            auto generation = x_imap(x);
            auto index = std::lround(generation + frame_count);
            draw_metric(width, index);
        }
    }
    cache_painter.end();

    if (s.cache_wrapped)
    {
        int cache_head_width = s.cache_end;
        int cache_tail_x = cache_head_width;
        int cache_tail_width = rc.width() - s.cache_end;
        if (cache_tail_width > 0)
            painter.drawPixmap(
                rc.topLeft(), s.cache_image,
                QRect{ cache_tail_x, 0, cache_tail_width, rc.height() });
        if (cache_head_width > 0)
            painter.drawPixmap(
                QPoint{ rc.left() + cache_tail_width, rc.top() }, s.cache_image,
                QRect{ 0, 0, cache_head_width, rc.height() });
    }
    else
    {
        int cache_head_width = s.cache_end;
        painter.drawPixmap(
            QPoint{ rc.right() - cache_head_width, rc.top() }, s.cache_image,
            QRect{ 0, 0, cache_head_width, rc.height() });
    }

    axes_painter.draw(painter);
}

} // namespace plot
