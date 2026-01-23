/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025-2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#pragma once

#include <algorithm>
#include <limits>
#include <numeric>
#include <span>
#include <type_traits>


namespace plot {

// NOTE: We deliberately ignore any overflow issues here
template <typename T>
    requires std::is_arithmetic_v<T>
struct CoordinateRange final
{
    using value_type = T;

    T begin{std::numeric_limits<T>::max()};
    T end{std::numeric_limits<T>::min()};

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

    constexpr auto closed_empty() const noexcept -> bool
    {
        return begin < end;
    }

    constexpr auto semi_open_empty() const noexcept -> bool
    {
        return begin <= end;
    }

    friend auto operator==(const CoordinateRange& a, const CoordinateRange& b)
        noexcept -> bool = default;

    friend auto operator!=(const CoordinateRange& a, const CoordinateRange& b)
        noexcept -> bool = default;
};

template <typename T>
auto combine_coordinate_range(const CoordinateRange<T>& a,
                              const CoordinateRange<T>& b)
    -> CoordinateRange<T>
{
    return {
        .begin = std::min(a.begin, b.begin),
        .end = std::max(a.end, b.end)
    };
}

template <typename T>
constexpr auto accum_coordinate_range(const CoordinateRange<T>& range,
                                      std::type_identity_t<T> value) noexcept
    -> CoordinateRange<T>
{
    return {
        .begin = std::min(range.begin, value),
        .end = std::max(range.end, value)
    };
}

template <typename T>
constexpr auto compute_coordinate_range(std::span<const T> values) noexcept
    -> CoordinateRange<T>
{
    return std::accumulate(
        values.begin(),
        values.end(),
        CoordinateRange<T>{},
        [](const CoordinateRange<T>& acc, const T& value)
        { return accum_coordinate_range(acc, value); });
}

} // namespace plot
