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

#include "plot_visual/color.hpp"
#include "plot_visual/opengl/time_series_visualizer.hpp"
#include "plot_visual/painter/time_series_histogram_visualizer.hpp"
#include "plot_visual/painter/time_series_visualizer.hpp"

#include "gc_types/live_time_series.hpp"

#include "gc/value.hpp"

#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <QPainter>
#include <QStackedLayout>

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

// ---

struct MetricData
{
    MetricData(size_t frame_capacity,
               QString x_label,
               QString y_label,
               QString title,
               QColor (*default_palette_color)(int index, int state_count),
               bool strict_palette_match) :
        default_palette_color{default_palette_color},
        strict_palette_match{strict_palette_match}
    {
        ts.set_frame_capacity(frame_capacity);
        vis_attr.x_label = std::move(x_label);
        vis_attr.y_label = std::move(y_label);
        vis_attr.title = std::move(title);
    }

    gc_types::LiveTimeSeries ts;
    plot::VisualizerAttributes vis_attr;

    QColor (*default_palette_color)(int index, int state_count);
    bool strict_palette_match;

    auto state_count() const noexcept -> size_t
    {
        auto frames = ts.frames();
        return frames.empty() ? 0ull : frames.front().values.size();
    }
};

// ---

template <typename Visualizer>
class PainterMetricWidget : public QWidget
{
public:
    template <typename... Args>
    explicit PainterMetricWidget(
        MetricData& data,
        std::optional<gc_types::IndexedPalette>& palette,
        QWidget* parent,
        Args&&... args) :
        QWidget{parent},
        data_{data},
        palette_{palette},
        vis_{data.ts, data.vis_attr, std::forward<Args>(args)...}
    {}

protected:
    auto paintEvent(QPaintEvent*) -> void override
    {
        auto state_count = data_.state_count();
        if (state_count == 0)
            return;

        data_.vis_attr.palette = ensure_palette(
            palette_,
            state_count,
            data_.default_palette_color,
            data_.strict_palette_match);

        auto painter = QPainter(this);
        plot::painter::safe_paint(&vis_, rect(), painter);
    }

private:
    MetricData& data_;
    std::optional<gc_types::IndexedPalette>& palette_;
    Visualizer vis_;
};

// ---

class OpenGLMetricWidget : public QOpenGLWidget,
                           protected QOpenGLFunctions_3_3_Core
{
public:
    explicit OpenGLMetricWidget(
        MetricData& data,
        std::optional<gc_types::IndexedPalette>& palette,
        QWidget* parent = nullptr) :
        QOpenGLWidget{parent},
        data_{data},
        palette_{palette},
        vis_{data.ts, data.vis_attr}
    {
        auto fmt = format();
        fmt.setSamples(4);
        setFormat(fmt);
    }

protected:
    auto initializeGL() -> void override
    {
        initializeOpenGLFunctions();
        vis_.bind_opengl_widget(*this, *this);
    }

    auto paintGL() -> void override
    {
        auto state_count = data_.state_count();
        if (state_count > 0)
        {
            data_.vis_attr.palette = ensure_palette(
                palette_,
                state_count,
                data_.default_palette_color,
                data_.strict_palette_match);
        }

        QPainter painter{this};
        painter.setRenderHint(QPainter::Antialiasing);
        plot::opengl::safe_paint(&vis_, rect(), painter);
    }

private:
    MetricData& data_;
    std::optional<gc_types::IndexedPalette>& palette_;
    plot::opengl::TimeSeriesVisualizer vis_;
};

} // anonymous namespace

struct ImageMetricsView::Storage
{
    explicit Storage(size_t frame_capacity,
                     const MetricRenderers& renderers) :
        renderers{renderers},
        state_histogram{
            /* frame_capacity */        frame_capacity,
            /* x_label */               "generation",
            /* y_label */               "state fraction",
            /* title */                 "State histogram",
            /* default_palette_color */ default_hue_color,
            /* strict_palette_match */  false},
        edge_histogram{
            /* frame_capacity */        frame_capacity,
            /* x_label */               "generation",
            /* y_label */               "diff. fraction",
            /* title */                 "Edge histogram",
            /* default_palette_color */ default_alternating_color,
            /* strict_palette_match */  true},
        plateau_avg_size{
            /* frame_capacity */        frame_capacity,
            /* x_label */               "generation",
            /* y_label */               "plateau size",
            /* title */                 "Average plateau size",
            /* default_palette_color */ default_hue_color,
            /* strict_palette_match */  false}
    {}

    auto clear() -> void
    {
        state_histogram.ts.clear();
        edge_histogram.ts.clear();
        plateau_avg_size.ts.clear();
    }

    MetricRenderers renderers;

    sieve::ImageMetric type{sieve::ImageMetric::StateHistogram};
    std::optional<gc_types::IndexedPalette> state_palette;
    std::optional<gc_types::IndexedPalette> edge_palette;

    MetricData state_histogram;
    MetricData edge_histogram;
    MetricData plateau_avg_size;

    QWidget* metric_widgets[3]{};
    QStackedLayout* stack_layout{};
};

ImageMetricsView::ImageMetricsView(size_t buf_size,
                                   const MetricRenderers& renderers,
                                   QWidget* parent):
    QWidget{ parent },
    storage_{std::make_unique<Storage>(buf_size, renderers)}
{
    auto* layout = new QStackedLayout(this);

    // StateHistogram widget
    storage_->metric_widgets[0] =
        new PainterMetricWidget<plot::painter::TimeSeriesHistogramVisualizer>(
            storage_->state_histogram, storage_->state_palette, this);

    // EdgeHistogram widget
    storage_->metric_widgets[1] =
        new PainterMetricWidget<plot::painter::TimeSeriesHistogramVisualizer>(
            storage_->edge_histogram, storage_->edge_palette, this);

    // PlateauAvgSize widget — depends on renderer
    switch (renderers.plateau_avg_size)
    {
    case plot::TimeSeriesRenderer::PainterFull:
        storage_->metric_widgets[2] =
            new PainterMetricWidget<plot::painter::TimeSeriesVisualizer>(
                storage_->plateau_avg_size, storage_->state_palette,
                this, false);
        break;
    case plot::TimeSeriesRenderer::PainterIncremental:
        storage_->metric_widgets[2] =
            new PainterMetricWidget<plot::painter::TimeSeriesVisualizer>(
                storage_->plateau_avg_size, storage_->state_palette,
                this, true);
        break;
    case plot::TimeSeriesRenderer::OpenGL:
        storage_->metric_widgets[2] =
            new OpenGLMetricWidget(
                storage_->plateau_avg_size, storage_->state_palette, this);
        break;
    }

    for (auto* w : storage_->metric_widgets)
        layout->addWidget(w);

    layout->setCurrentIndex(static_cast<int>(storage_->type));
    storage_->stack_layout = layout;
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
    storage_->metric_widgets[storage_->stack_layout->currentIndex()]->update();
}

auto ImageMetricsView::clear() -> void
{
    storage_->clear();
    storage_->metric_widgets[storage_->stack_layout->currentIndex()]->update();
}

auto ImageMetricsView::set_palette(const gc_types::IndexedPalette& palette)
    -> void
{
    storage_->state_palette = palette;
    storage_->metric_widgets[storage_->stack_layout->currentIndex()]->update();
}

auto ImageMetricsView::set_type(sieve::ImageMetric type) -> void
{
    storage_->type = type;
    storage_->stack_layout->setCurrentIndex(static_cast<int>(type));
    storage_->metric_widgets[static_cast<int>(type)]->update();
}

auto ImageMetricsView::set_type(const gc::Value& v) -> void
{
    set_type(v.as<sieve::ImageMetric>());
}
