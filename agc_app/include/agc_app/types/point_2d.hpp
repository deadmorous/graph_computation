#pragma once

#include <array>
#include <cstdint>


namespace agc_app {

template <typename T>
using Point2d = std::array<T, 2>;

using Point2d_u32 = Point2d<uint32_t>;

using Point2d_d = Point2d<double>;

} // namespace agc_app
