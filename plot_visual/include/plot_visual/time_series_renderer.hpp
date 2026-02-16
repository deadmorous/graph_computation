/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "plot_visual/rendering_engine.hpp"

#include <cstdint>


namespace plot {

enum class TimeSeriesRenderer : uint8_t
{
    PainterFull,
    PainterIncremental,
    OpenGL
};

auto rendering_engine(TimeSeriesRenderer renderer) -> RenderingEngine;

enum class TimeSeriesHistogramRenderer : uint8_t
{
    PainterIncremental
};

auto rendering_engine(TimeSeriesHistogramRenderer renderer) -> RenderingEngine;

} // namespace plot
