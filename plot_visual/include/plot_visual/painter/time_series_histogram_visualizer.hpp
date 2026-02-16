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

#include "plot_visual/painter/visualizer.hpp"

#include "gc_types/live_time_series_fwd.hpp"
#include "plot_visual/visualizer_attributes.hpp"

#include <memory>


namespace plot::painter {

class TimeSeriesHistogramVisualizer final : public Visualizer
{
public:
    using Attributes = plot::VisualizerAttributes;

    ~TimeSeriesHistogramVisualizer();

    // NOTE
    // Accept non-const LiveTimeSeries& because of checkpointing
    // Accept non-const Attributes& to avoid passing a temporary
    TimeSeriesHistogramVisualizer(
        gc_types::LiveTimeSeries&, Attributes& attributes);

    auto paint(const QRect&, QPainter&) -> void override;

private:
    struct Storage;
    std::unique_ptr<Storage> storage_;
};

} // namespace plot::painter
