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
#include "gc_types/palette.hpp"

namespace plot::painter {

class TimeSeriesVisualizer final : public Visualizer
{
public:
    struct Attributes final
    {
        gc_types::IndexedPalette palette;
        QString x_label;
        QString y_label;
        QString title;
        bool incremental{true};
    };

    ~TimeSeriesVisualizer();

    // NOTE
    // Accept non-const LiveTimeSeries& because of checkpointing
    // Accept non-const Attributes& to avoid passing a temporary
    TimeSeriesVisualizer(gc_types::LiveTimeSeries&, Attributes& attributes);

    auto paint(const QRect&, QPainter&) -> void override;

    struct Storage;

private:
    std::unique_ptr<Storage> storage_;
};

} // namespace plot::painter
