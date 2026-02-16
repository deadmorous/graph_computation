/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025-2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#pragma once

#include "sieve/types/image_metrics.hpp"

#include "plot_visual/time_series_renderer.hpp"

#include "gc_types/palette.hpp"

#include "gc/value_fwd.hpp"

#include <QImage>
#include <QWidget>

struct MetricRenderers
{
    plot::TimeSeriesHistogramRenderer state_histogram
        {plot::TimeSeriesHistogramRenderer::PainterIncremental};
    plot::TimeSeriesHistogramRenderer edge_histogram
        {plot::TimeSeriesHistogramRenderer::PainterIncremental};
    plot::TimeSeriesRenderer plateau_avg_size
        {plot::TimeSeriesRenderer::OpenGL};
};

class ImageMetricsView
    : public QWidget
{
    Q_OBJECT

public:
    explicit ImageMetricsView(size_t buf_size,
                              const MetricRenderers& renderers = {},
                              QWidget* parent = nullptr);

    ~ImageMetricsView();

    auto type() const noexcept -> sieve::ImageMetric;

public slots:
    auto add_image_metrics(const sieve::ImageMetrics& image_metrics) -> void;
    auto clear() -> void;
    auto set_palette(const gc_types::IndexedPalette&) -> void;
    auto set_type(sieve::ImageMetric) -> void;
    auto set_type(const gc::Value& v) -> void;

protected:
    auto paintEvent(QPaintEvent*) -> void override;

public:
    struct Storage;

private:
    std::unique_ptr<Storage> storage_;
};
