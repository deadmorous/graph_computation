/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#pragma once

#include <type_traits>


namespace plot {

// NOTE: We deliberately ignore any overflow issues here
template <typename T>
    requires std::is_arithmetic_v<T>
struct CoordinateRange final
{
    using value_type = T;

    T begin;
    T end;

    constexpr auto length() const noexcept -> T
    { return end - begin; }

    constexpr auto closed_contains(T x) const noexcept -> bool
    {
        return begin <= x && x <= end;
    }

    constexpr auto semi_open_contains(T x) const noexcept -> bool
    {
        return begin <= x && x < end;
    }
};

} // namespace plot
