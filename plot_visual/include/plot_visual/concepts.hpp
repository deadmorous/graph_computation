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

#include <concepts>
#include <span>
#include <type_traits>

namespace plot {

template <typename T>
concept NumericType =
    std::is_arithmetic_v<T>;

template <typename T>
concept CoordinateMappingType = requires(const T& mapping)
{
    typename T::From;
    requires NumericType<typename T::From>;

    typename T::To;
    requires NumericType<typename T::To>;

    { mapping(typename T::From{}) } -> std::same_as<typename T::To>;

    { mapping.ticks(std::span<typename T::From>{}) }
        -> std::convertible_to<std::span<const typename T::From>>;
};

template <typename T>
concept AxisType = requires(T axis)
{
    typename T::CoordinateMapping;
    requires CoordinateMappingType<typename T::CoordinateMapping>;

    { axis.mapping } -> std::same_as<typename T::CoordinateMapping&>;
    axis.primary_ticks();
    axis.secondary_ticks();
};

template <typename T>
concept Axes2dType = requires(T axes)
{
    typename T::XAxis;
    requires AxisType<typename T::XAxis>;

    typename T::YAxis;
    requires AxisType<typename T::YAxis>;

    { axes.x } -> std::same_as<typename T::XAxis&>;
    { axes.y } -> std::same_as<typename T::YAxis&>;
};

} // namespace plot
