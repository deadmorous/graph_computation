/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include <array>
#include <cstdint>


namespace agc_app_rt {

template <typename T>
using Point2d = std::array<T, 2>;

using Point2d_u32 = Point2d<uint32_t>;

using Point2d_d = Point2d<double>;

} // namespace agc_app_rt
