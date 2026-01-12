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

#include "gc_visual/plot/coordinate_range.hpp"

#include <cmath>
#include <span>


namespace plot {

template <typename From_, typename To_>
struct LinearCoordinateMapping final
{
    using From = From_;
    using To = To_;

    CoordinateRange<From> from;
    CoordinateRange<To> to;

    auto operator()(From x) const -> To
    {
        return static_cast<To>(
            to.begin + (x - from.begin) * to.length() / from.length());
    }

    auto ticks(std::span<From> buf) const noexcept -> std::span<From>
    {
        auto n = static_cast<From>(buf.size());
        if (n == 0)
            return {};

        auto d = std::abs(from.length());
        if (d == 0)
            return {};

        auto ceil = [](From x, From unit)
        {
            return static_cast<From>(std::ceil(x / unit) * unit);
        };

        auto floor = [](From x, From unit)
        {
            return static_cast<From>(std::floor(x / unit) * unit);
        };

        auto count_ticks = [](From diff, From unit)
        {
            return std::lround(diff / unit) + 1;
        };

        auto try_unit = [&](From unit) -> std::span<From>
        {
            if (from.begin < from.end)
            {
                auto begin = ceil(from.begin, unit);
                auto end = floor(from.end, unit);
                auto tick_count = count_ticks(end - begin, unit);
                if (tick_count < 0 || tick_count > n)
                    return {};
                for (int i=0; i<tick_count; ++i, begin+=unit)
                    buf[i] = begin;
                return buf.subspan(0, tick_count);
            }
            else
            {
                auto begin = floor(from.begin, unit);
                auto end = ceil(from.end, unit);
                auto tick_count = count_ticks(begin - end, unit);
                if (tick_count < 0 || tick_count > n)
                    return {};
                for (int i=0; i<tick_count; ++i, begin-=unit)
                    buf[i] = begin;
                return buf.subspan(0, tick_count);
            }
        };

        auto unit = static_cast<From>(pow(10, std::ceil(log10(d))));
        auto best_result = std::span<From>{};
        while (true)
        {
            if (unit == 0)
                return best_result;
            for (auto divisor : {1, 2, 5})
            {
                auto result = try_unit(unit / divisor);
                if (result.empty())
                {
                    if (!best_result.empty())
                        return best_result;
                }
                else if (result.size() == buf.size())
                    return result;
                else
                    best_result = result;
            }
            unit /= 10;
        }
    }

    auto inverse() const noexcept -> LinearCoordinateMapping<To, From>
    {
        return { .from = to, .to = from };
    }
};

} // namespace plot
