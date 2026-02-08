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

#include "plot_visual/axis.hpp"
#include "plot_visual/axes_2d.hpp"
#include "plot_visual/color.hpp"
#include "plot_visual/linear_coordinate_mapping.hpp"
#include "plot_visual/layout.hpp"
#include "plot_visual/painter/time_series_histogram_visualizer.hpp"
#include "plot_visual/painter/time_series_visualizer.hpp"

#include "gc_types/live_time_series.hpp"

#include "gc/value.hpp"

#include <QPainter>
#include <QPainterPath>

#include <deque>


template <typename Visualizer>
struct VisualizationData
{
    explicit VisualizationData(size_t frame_capacity)
    { ts.set_frame_capacity(frame_capacity); }

    gc_types::LiveTimeSeries ts;
    Visualizer::Attributes ts_vis_attr;
    Visualizer ts_vis{ts, ts_vis_attr};

    auto state_count() const noexcept -> size_t
    {
        auto frames = ts.frames();
        return frames.empty() ? 0ull : frames.front().values.size();
    }
};

using HistogramVisualizationData =
    VisualizationData<plot::TimeSeriesHistogramVisualizer>;

using PlotVisualizationData = VisualizationData<plot::TimeSeriesVisualizer>;

struct ImageMetricsView::Storage
{
    explicit Storage(size_t frame_capacity) :
        state_histogram{frame_capacity},
        edge_histogram{frame_capacity},
        plateau_avg_size{frame_capacity}
    {}

    auto clear() -> void
    {
        state_histogram.ts.clear();
        edge_histogram.ts.clear();
        plateau_avg_size.ts.clear();
    }

    sieve::ImageMetric type{sieve::ImageMetric::StateHistogram};
    std::optional<gc_types::IndexedPalette> state_palette;
    std::optional<gc_types::IndexedPalette> edge_palette;

    HistogramVisualizationData state_histogram;
    HistogramVisualizationData edge_histogram;
    PlotVisualizationData plateau_avg_size;
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

auto ensure_palette(std::optional<gc_types::IndexedPalette>& palette,
                    int state_count,
                    auto color_map_func,
                    bool strict_palette_match) -> gc_types::IndexedPalette&
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
        pal.color_map.push_back(plot::color(qcolor));
    }

    return pal;
}

struct MetricViewData
{
    std::vector<double> sieve::ImageMetrics::* metric;
    std::optional<gc_types::IndexedPalette> ImageMetricsView::Storage::* palette;
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
                           HistogramVisualizationData& v,
                           const QRect& rect,
                           QPainter& painter)
    -> void
{
    auto state_count = v.state_count();
    if (state_count == 0)
        return;

    auto& pal = ensure_palette(
        s.*(d.palette),
        state_count,
        d.default_palette_color,
        d.strict_palette_match);

    v.ts_vis_attr.palette = pal;
    v.ts_vis_attr.x_label = d.x_label;
    v.ts_vis_attr.y_label = d.y_label;
    v.ts_vis_attr.title = d.title;

    plot::safe_paint(&v.ts_vis, rect, painter);
}

auto plot_time_series_metric(const MetricViewData& d,
                             ImageMetricsView::Storage& s,
                             PlotVisualizationData& v,
                             const QRect& rect,
                             QPainter& painter)
    -> void
{
    auto state_count = v.state_count();
    if (state_count == 0)
        return;

    auto& pal = ensure_palette(
        s.*(d.palette),
        state_count,
        d.default_palette_color,
        d.strict_palette_match);

    v.ts_vis_attr.palette = pal;
    v.ts_vis_attr.x_label = d.x_label;
    v.ts_vis_attr.y_label = d.y_label;
    v.ts_vis_attr.title = d.title;

    plot::safe_paint(&v.ts_vis, rect, painter);
}

[[maybe_unused]]
auto plot_metric(ImageMetricsView::Storage& s,
                 const QRect& rect,
                 QPainter& painter)
    -> void
{
    auto d = metric_view_data(s.type);
    switch (s.type)
    {
    case sieve::ImageMetric::StateHistogram:
        plot_histogram_metric(d, s, s.state_histogram, rect, painter);
        break;
    case sieve::ImageMetric::EdgeHistogram:
        plot_histogram_metric(d, s, s.edge_histogram, rect, painter);
        break;
    case sieve::ImageMetric::PlateauAvgSize:
        plot_time_series_metric(d, s, s.plateau_avg_size, rect, painter);
        break;
    }
}

} // anonymous namespace

ImageMetricsView::ImageMetricsView(size_t buf_size, QWidget* parent):
    QWidget{ parent },
    storage_{std::make_unique<Storage>(buf_size)}
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
    storage_->state_histogram.ts.add(image_metrics.histogram);
    storage_->edge_histogram.ts.add(image_metrics.edge_histogram);
    storage_->plateau_avg_size.ts.add(image_metrics.plateau_avg_size);
    update();
}

auto ImageMetricsView::clear() -> void
{
    storage_->clear();
    update();
}

auto ImageMetricsView::set_palette(const gc_types::IndexedPalette& palette)
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
