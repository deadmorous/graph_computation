#pragma once

#include "agc_app/types/point_2d.hpp"

#include "common/struct_type_macro.hpp"

#include <ostream>
#include <vector>


namespace agc_app {

using CanvasSize =
    Point2d_u32;

template <typename T>
struct Canvas
{
    CanvasSize size;
    std::vector<T> pixels;
};

GCLIB_STRUCT_TYPE(Canvas<double>, size, pixels);


template <typename T>
struct CanvasPixel
{
    Point2d_u32 pos;
    T value;
};

GCLIB_STRUCT_TYPE(CanvasPixel<double>, pos, value);

auto operator<<(std::ostream& s, const Canvas<double>& canvas)
    -> std::ostream&;

} // namespace agc_app
