#pragma once

#include "common/struct_type_macro.hpp"

#include <array>
#include <cstdint>
#include <vector>


namespace agc_app {

template <typename T>
struct Canvas
{
    std::array<uint32_t, 2> size;
    std::vector<T> pixels;
};

GCLIB_STRUCT_TYPE(Canvas<double>, size, pixels);


template <typename T>
struct CanvasPixel
{
    std::array<uint32_t, 2> pos;
    T value;
};

GCLIB_STRUCT_TYPE(CanvasPixel<double>, pos, value);

} // namespace agc_app
