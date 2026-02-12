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
#include "plot_visual/layout.hpp"
#include "plot_visual/linear_coordinate_mapping.hpp"
#include "plot_visual/painter/detail/x_incremental_drawing.hpp"

#include <QPainterPath>

namespace plot {

using namespace gc_types;

namespace {

auto make_pen(const IndexedPalette palette, int state) -> QPen
{
    constexpr int pen_width = 2;
    auto pen_color = qcolor(map_color(palette, state));
    auto pen_style = Qt::SolidLine;
    auto lightness = pen_color.lightnessF();
    constexpr auto lightness_threshold = 0.9;   // TODO better
    if (lightness > lightness_threshold)
    {
        lightness /= 2;
        pen_style = Qt::DashLine;
        pen_color = QColor::fromHslF(
            pen_color.hslHue(),
            pen_color.hslSaturationF(),
            lightness);
    }
    return QPen(pen_color, pen_width, pen_style);
}

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

    plot::detail::XIncrementalDrawing drawing;
    CoordinateRange<double> y_range;
};

namespace {

auto paint_full(TimeSeriesVisualizer::Storage& s,
                const QRect& rect,
                QPainter& painter)
    -> void
{
    auto frames = s.time_series.frames();
    if (frames.empty())
        return;

    using CoordMap = plot::LinearCoordinateMapping<double, double>;
    using Axis = plot::Axis<CoordMap>;
    using Axes = plot::Axes2d<Axis, Axis>;

    auto frame_capacity = s.time_series.frame_capacity();
    int frame_count = frames.size();    // NOTE: Signedness is important
    auto state_count = frames.front().values.size();

    auto y_max = s.time_series.value_range().end;

    auto axes_painter = plot::AxesPainter{
        Axes{
            .x = {
                .mapping{
                    .from{
                        .begin = -static_cast<double>(frame_capacity),
                        .end = 0}},
                .label = s.attributes.x_label },
            .y = {
                .mapping{ .from = { .begin = 0., .end = y_max }},
                .label = s.attributes.y_label },
            .title = s.attributes.title
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
            painter.strokePath(path, make_pen(s.attributes.palette, state));
    }

    axes_painter.draw(painter);
}

auto paint_incremental(
    TimeSeriesVisualizer::Storage& s, const QRect& rect, QPainter& painter)
    -> void
{
    auto frames = s.time_series.frames();
    if (frames.empty())
        return;

    using CoordMap = plot::LinearCoordinateMapping<double, double>;
    using Axis = plot::Axis<CoordMap>;
    using Axes = plot::Axes2d<Axis, Axis>;

    auto frame_capacity = s.time_series.frame_capacity();
    int frame_count = frames.size();    // NOTE: Signedness is important
    auto state_count = frames.front().values.size();

    auto scale_changed = false;

    constexpr auto range_jump_factor = 1.2;
    if (auto y_max = s.time_series.value_range().end;
        s.y_range.closed_contains(y_max))
    {
        if (y_max * range_jump_factor < s.y_range.end)
        {
            s.y_range = { 0, y_max };
            scale_changed = true;
        }
    }
    else
    {
        s.y_range = { 0, y_max * range_jump_factor };
        scale_changed = true;
    }

    auto axes_painter = plot::AxesPainter{
        Axes{
            .x = {
                .mapping{
                    .from{
                        .begin = -static_cast<double>(frame_capacity),
                        .end = 0}},
                .label = s.attributes.x_label },
            .y = {
                .mapping{ .from = s.y_range },
                .label = s.attributes.y_label },
            .title = s.attributes.title
        },
        rect
    };
    const auto& axes = axes_painter.axes();

    auto rc = axes_painter.layout().rect(plot::layout::central);

    bool cache_valid = !scale_changed &&
                       !s.drawing.empty() &&
                       s.drawing.size() == rc.size();

    if (!cache_valid)
        s.drawing.resize(rc.size(), painter.device()->devicePixelRatioF());

    using DrawingUpdater = detail::XIncrementalDrawing::Updater;

    s.drawing.update([&](DrawingUpdater& updater){
        auto frames_added =
            s.checkpoint.sync().frames_added.value_or(frame_count);
        int first_index = cache_valid ? frame_count - frames_added : 0;
        if (first_index == 0)
            ++first_index;
        if (first_index >= frame_count)
            return;

        auto frame_width =
            static_cast<double>(rc.width()) / (frame_capacity - 1);
        for (int frame_index=first_index; frame_index<frame_count; ++frame_index)
        {
            updater(
                frame_width,
                true,
                [&](QPainter& drawing_painter, double x0)
                {
                    drawing_painter.fillRect(
                        QRectF{x0,
                               0,
                               frame_width,
                               static_cast<double>(rc.height())},
                        Qt::white);

                    drawing_painter.setRenderHint(QPainter::Antialiasing);
                    for (size_t state=0; state<state_count; ++state)
                    {
                        drawing_painter.setPen(
                            make_pen(s.attributes.palette, state));

                        auto frame_mapped_y = [&](int frame_index)
                            -> std::optional<double>
                        {
                            const auto& frame = frames[frame_index];
                            if (frame.values.size() <= state)
                                return std::nullopt;
                            auto y = frame.values[state];
                            auto mapped_y = axes.y.mapping(y) - rc.top();
                            return mapped_y;
                        };

                        auto y0 = frame_mapped_y(frame_index - 1);
                        auto y1 = frame_mapped_y(frame_index);

                        if (y0.has_value() && y1.has_value())
                        {
                            auto x1 = x0 + frame_width;
                            drawing_painter.drawLine(
                                QPointF{x0, *y0}, QPointF{x1, *y1});
                        }
                    }

                });
        }
    });

    // Uncomment to debug
    // painter.fillRect(rc.adjusted(-1, -1, 1, 1), Qt::red);
    // painter.fillRect(rc, Qt::yellow);

    s.drawing.draw(painter, rc.topLeft());
    axes_painter.draw(painter);
}

} // anonymous namespace


TimeSeriesVisualizer::~TimeSeriesVisualizer() = default;

TimeSeriesVisualizer::TimeSeriesVisualizer(LiveTimeSeries& time_series,
                                           Attributes& attributes) :
    storage_{std::make_unique<Storage>(time_series, attributes) }
{}

auto TimeSeriesVisualizer::paint(const QRect& rect, QPainter& painter) -> void
{
    auto& s = *storage_;

    if (s.attributes.incremental)
        paint_incremental(s, rect, painter);
    else
        paint_full(s, rect, painter);

}

} // namespace plot
