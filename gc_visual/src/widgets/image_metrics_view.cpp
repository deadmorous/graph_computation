/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025-2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#include "gc_visual/widgets/image_metrics_view.hpp"

#include "gc_visual/color.hpp"
#include "plot_visual/axis.hpp"
#include "plot_visual/axes_2d.hpp"
#include "plot_visual/linear_coordinate_mapping.hpp"
#include "plot_visual/layout.hpp"

#include "gc/value.hpp"

#include <QPainter>
#include <QPainterPath>

#include <algorithm>
#include <deque>


struct ImageMetricsView::Storage
{
    using Buffer = std::deque<sieve::ImageMetrics>;

    size_t buf_size{};
    Buffer buf;
    sieve::ImageMetric type{sieve::ImageMetric::StateHistogram};
    std::optional<gc_app::IndexedPalette> state_palette;
    std::optional<gc_app::IndexedPalette> edge_palette;
};

namespace {

[[maybe_unused]]
auto default_hue_color(int index, int state_count) -> QColor
{
    return QColor::fromHsvF(static_cast<float>(index)/state_count, 1.f, 1.f);
}

[[maybe_unused]]
auto default_grayscale_color(int index, int state_count) -> QColor
{
    return QColor::fromHslF(
        1.f, 0.f, static_cast<float>(index)/(state_count-1));
}

[[maybe_unused]]
auto default_alternating_color(int index, int state_count) -> QColor
{
    auto p = static_cast<float>(index)/(state_count-1);
    if (state_count > 8 && index & 1)
        return QColor::fromHsvF(p, 1.f, 1.0f);
    else
        return QColor::fromHslF(1.f, 0.f, p);
}

auto ensure_palette(std::optional<gc_app::IndexedPalette>& palette,
                    int state_count,
                    auto color_map_func,
                    bool strict_palette_match) -> gc_app::IndexedPalette&
{
    if (palette)
    {
        if (!strict_palette_match ||
            palette->color_map.size() == static_cast<size_t>(state_count))
            return *palette;
    }

    auto& pal = palette.emplace();

    pal.color_map.reserve(state_count);
    for (int i=0; i<state_count; ++i)
    {
        auto qcolor = color_map_func(i, state_count);
        pal.color_map.push_back(gc_visual::color(qcolor));
    }

    return pal;
}

enum class PlotMode : uint8_t
{
    HistogramHistory,
    TimeSeries
};

struct MetricViewData
{
    std::vector<double> sieve::ImageMetrics::* metric;
    PlotMode plot_mode;
    std::optional<gc_app::IndexedPalette> ImageMetricsView::Storage::* palette;
    QColor (*default_palette_color)(int index, int state_count);
    bool strict_palette_match;
    QString x_label;
    QString y_label;
    QString title;
};

auto metric_view_data(sieve::ImageMetric type) -> MetricViewData
{
    switch(type)
    {
    case sieve::ImageMetric::StateHistogram:
        return {
            .metric = &sieve::ImageMetrics::histogram,
            .plot_mode = PlotMode::HistogramHistory,
            .palette = &ImageMetricsView::Storage::state_palette,
            .default_palette_color = default_hue_color,
            .strict_palette_match = false,
            .x_label = "generation",
            .y_label = "state fraction",
            .title = "State histogram"
        };
    case sieve::ImageMetric::EdgeHistogram:
        return {
            .metric = &sieve::ImageMetrics::edge_histogram,
            .plot_mode = PlotMode::HistogramHistory,
            .palette = &ImageMetricsView::Storage::edge_palette,
            .default_palette_color = default_alternating_color,
            .strict_palette_match = true,
            .x_label = "generation",
            .y_label = "diff. fraction",
            .title = "Edge histogram"
        };
    case sieve::ImageMetric::PlateauAvgSize:
        return {
            .metric = &sieve::ImageMetrics::plateau_avg_size,
            .plot_mode = PlotMode::TimeSeries,
            .palette = &ImageMetricsView::Storage::state_palette,
            .default_palette_color = default_hue_color,
            .strict_palette_match = false,
            .x_label = "generation",
            .y_label = "plateau size",
            .title = "Average plateau size"
        };
    }
    __builtin_unreachable();
}

auto plot_histogram_metric(const MetricViewData& d,
                           ImageMetricsView::Storage& s,
                           const QRect& rect,
                           QPainter& painter)
    -> void
{
    auto generation_count = s.buf.size();

    if (generation_count == 0)
        return;

    using CoordMap = plot::LinearCoordinateMapping<double, int>;
    using Axis = plot::Axis<CoordMap>;
    using Axes = plot::Axes2d<Axis, Axis>;

    auto axes_painter = plot::AxesPainter{
        Axes{
            .x = {
                .mapping{
                    .from{
                        .begin = -static_cast<double>(s.buf_size),
                        .end = 0}},
                .label = d.x_label },
            .y = {
                .mapping{ .from = { .begin = 0., .end = 1. }},
                .label = d.y_label },
            .title = d.title
        },
        rect
    };
    const auto& axes = axes_painter.axes();

    auto rc = axes_painter.layout().rect(plot::layout::central);

    auto metric = [&](size_t index) -> const std::vector<double>&
    {
        return s.buf[index].*(d.metric);
    };

    auto& pal = ensure_palette(
        s.*(d.palette),
        metric(generation_count-1).size(),
        d.default_palette_color,
        d.strict_palette_match);

    auto draw_metric = [&](int x, int index)
    {
        const auto& buf = s.buf;
        if (static_cast<size_t>(index) >= buf.size())
            return;
        const auto& m = metric(index);

        auto p0 = double{0};
        auto y0 = axes.y.mapping(p0);
        auto n = m.size();
        for (size_t i=0; i<n; ++i)
        {
            auto p1 = p0 + m[i];
            auto y1 = axes.y.mapping(p1);
            auto color = gc_visual::qcolor(map_color(pal, i));
            painter.fillRect(x, y0, 1, y1-y0, color);
            p0 = p1;
            y0 = y1;
        }
    };

    auto draw_interpolated_metric = [&](int x, double generation)
    {
        auto index_plus_t = s.buf.size() - 1 + generation;
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

auto plot_time_series_metric(const MetricViewData& d,
                             ImageMetricsView::Storage& s,
                             const QRect& rect,
                             QPainter& painter)
    -> void
{
    int generation_count = s.buf.size();

    if (generation_count == 0)
        return;

    using CoordMap = plot::LinearCoordinateMapping<double, int>;
    using Axis = plot::Axis<CoordMap>;
    using Axes = plot::Axes2d<Axis, Axis>;

    auto metric = [&](size_t index) -> const std::vector<double>&
    {
        return s.buf[index].*(d.metric);
    };

    auto state_count = metric(generation_count-1).size();

    auto y_max = [&]{
        auto gen_range = common::index_range<size_t>(generation_count);
        return std::accumulate(
            gen_range.begin(), gen_range.end(), 0.,
            [&](double acc, size_t generation){
                const auto& m = metric(generation);
                auto n = std::min(m.size(), state_count);
                auto x = *std::max_element(m.begin(), m.begin()+n);
                return std::max(acc, x);
            });
        }();

    auto axes_painter = plot::AxesPainter{
        Axes{
            .x = {
                .mapping{
                    .from{
                        .begin = -static_cast<double>(s.buf_size),
                        .end = 0}},
                .label = d.x_label },
            .y = {
                .mapping{ .from = { .begin = 0., .end = y_max }},
                .label = d.y_label },
            .title = d.title
        },
        rect
    };
    const auto& axes = axes_painter.axes();

    [[maybe_unused]]
    auto rc = axes_painter.layout().rect(plot::layout::central);

    auto& pal = ensure_palette(
        s.*(d.palette),
        state_count,
        d.default_palette_color,
        d.strict_palette_match);

    painter.setRenderHint(QPainter::Antialiasing);

    for (size_t state=0; state<state_count; ++state)
    {
        auto path = QPainterPath{};
        auto path_started = false;
        for (int generation=0; generation<generation_count; ++generation)
        {
            const auto& m = metric(generation);
            if (m.size() <= state)
                continue;
            auto px = axes.x.mapping(generation - generation_count);
            auto py = axes.y.mapping(m[state]);
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
            auto pen_color = gc_visual::qcolor(map_color(pal, state));
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

auto plot_metric(ImageMetricsView::Storage& s,
                 const QRect& rect,
                 QPainter& painter)
    -> void
{
    auto d = metric_view_data(s.type);
    switch (d.plot_mode)
    {
    case PlotMode::HistogramHistory:
        plot_histogram_metric(d, s, rect, painter);
        break;
    case PlotMode::TimeSeries:
        plot_time_series_metric(d, s, rect, painter);
        break;
    }
}

} // anonymous namespace

ImageMetricsView::ImageMetricsView(size_t buf_size, QWidget* parent):
    QWidget{ parent },
    storage_{std::make_unique<Storage>(Storage{.buf_size = buf_size})}
{
}

ImageMetricsView::~ImageMetricsView() = default;

auto ImageMetricsView::type() const noexcept -> sieve::ImageMetric
{
    return storage_->type;
}

auto ImageMetricsView::add_image_metrics(
    const sieve::ImageMetrics& image_metrics) -> void
{
    auto& buf = storage_->buf;
    if (buf.size() == storage_->buf_size)
        buf.pop_front();
    buf.push_back(image_metrics);
    update();
}

auto ImageMetricsView::clear() -> void
{
    storage_->buf.clear();
    update();
}

auto ImageMetricsView::set_palette(const gc_app::IndexedPalette& palette)
    -> void
{
    storage_->state_palette = palette;
    update();
}

auto ImageMetricsView::set_type(sieve::ImageMetric type) -> void
{
    storage_->type = type;
    update();
}

auto ImageMetricsView::set_type(const gc::Value& v) -> void
{
    set_type(v.as<sieve::ImageMetric>());
}

auto ImageMetricsView::paintEvent(QPaintEvent*) -> void
{
    auto painter = QPainter(this);
    try {
        plot_metric(*storage_, rect(), painter);
    }
    catch (std::exception& e) {
        painter.setPen(Qt::red);
        painter.drawText(rect(), Qt::AlignHCenter | Qt::AlignVCenter, e.what());
    }
}
