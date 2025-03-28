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

#include <concepts>
#include <type_traits>


namespace common {

template <typename T, typename U>
concept MaybeConst =
    std::same_as<T, U> || std::same_as<T, const U>;

template <typename T, typename U>
using AsConstAs =
    std::conditional_t<
        std::is_const_v<T>,
        const std::remove_const_t<U>,
        std::remove_const_t<U>>;

} // namespace common
