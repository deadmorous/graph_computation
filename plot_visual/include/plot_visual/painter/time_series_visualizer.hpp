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

namespace plot::painter {

class TimeSeriesVisualizer final : public Visualizer
{
public:
    using Attributes = plot::VisualizerAttributes;

    ~TimeSeriesVisualizer();

    // NOTE
    // Accept non-const LiveTimeSeries& because of checkpointing
    // Accept non-const Attributes& to avoid passing a temporary
    TimeSeriesVisualizer(gc_types::LiveTimeSeries&, Attributes& attributes,
                         bool incremental = true);

    auto paint(const QRect&, QPainter&) -> void override;

    struct Storage;

private:
    std::unique_ptr<Storage> storage_;
};

} // namespace plot::painter
