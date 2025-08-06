#pragma once

#include "agc_rt/types/canvas.hpp"

#include <algorithm>


namespace agc_rt {

inline auto clear_canvas(Canvas<double>& canvas) noexcept
    -> void
{
    std::fill(canvas.pixels.begin(), canvas.pixels.end(), 0.);
    canvas.pos = 0;
}

inline auto resize_canvas(Canvas<double>& canvas, CanvasSize size)
    -> void
{
    canvas.size = size;
    canvas.pixels.resize(size[0]*size[1]);
    clear_canvas(canvas);
}

inline auto set_canvas_pixel(Canvas<double>& canvas,
                             const CanvasPixel<double>& pixel) noexcept
    -> void
{
    if (pixel.pos[0] < canvas.size[0] && pixel.pos[1] < canvas.size[1])
    {
        auto index = pixel.pos[0] + pixel.pos[1] * canvas.size[0];
        canvas.pixels[index] = pixel.value;
    }
}

inline auto set_next_canvas_pixel(Canvas<double>& canvas,
                                  double value) noexcept
    -> void
{
    if (canvas.pos < canvas.pixels.size())
        canvas.pixels[canvas.pos++] = value;
}

} // namespace agc_rt
