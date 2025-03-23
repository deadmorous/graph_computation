/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include <array>
#include <tuple>
#include <utility>


namespace common {

template<typename T>
constexpr inline bool is_tuple_like_v = false;

template <typename T, size_t size>
constexpr inline bool is_tuple_like_v<std::array<T, size>> = true;

template <typename T0, typename T1>
constexpr inline bool is_tuple_like_v<std::pair<T0, T1>> = true;

template <typename... Ts>
constexpr inline bool is_tuple_like_v<std::tuple<Ts...>> = true;

template <typename T>
concept tuple_like = is_tuple_like_v<T>;

} // namespace common
