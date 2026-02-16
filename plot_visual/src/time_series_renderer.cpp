/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "plot_visual/time_series_renderer.hpp"

#include <cstdint>


namespace plot {

auto rendering_engine(TimeSeriesRenderer renderer)
    -> RenderingEngine
{
    switch(renderer)
    {
    case TimeSeriesRenderer::PainterFull:
    case TimeSeriesRenderer::PainterIncremental:
        return RenderingEngine::Painter;
    case TimeSeriesRenderer::OpenGL:
        return RenderingEngine::OpenGL;
    }
    __builtin_unreachable();
}

auto rendering_engine(TimeSeriesHistogramRenderer renderer)
    -> RenderingEngine
{
    switch(renderer)
    {
    case TimeSeriesHistogramRenderer::PainterIncremental:
        return RenderingEngine::Painter;
    }
    __builtin_unreachable();
}

} // namespace plot
