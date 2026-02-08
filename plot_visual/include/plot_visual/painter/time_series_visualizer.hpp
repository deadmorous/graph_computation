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

#include "plot_visual/live_time_series_fwd.hpp"
#include "plot_visual/painter/visualizer.hpp"

#include "gc_app/types/palette.hpp"

namespace plot {

class TimeSeriesVisualizer final : public PainterVisualizer
{
public:
    struct Attributes final
    {
        gc_app::IndexedPalette palette;
        QString x_label;
        QString y_label;
        QString title;
    };

    // NOTE
    // Accept non-const LiveTimeSeries& because of checkpointing
    // Accept non-const Attributes& to avoid passing a temporary
    TimeSeriesVisualizer(LiveTimeSeries&, Attributes& attributes);

    auto paint(const QRect&, QPainter&) -> void override;

private:
    LiveTimeSeries* time_series_;
    const Attributes* attributes_;
};

} // namespace plot
