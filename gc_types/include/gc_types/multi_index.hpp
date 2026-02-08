/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024-2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#pragma once

#include "common/binomial.hpp"

#include <algorithm>
#include <concepts>
#include <cstdint>
#include <span>
#include <type_traits>
#include <vector>

namespace gc_types {

template <typename C, typename T>
concept MutableIndexedContainerOf =
    requires(C& c)
    {
        { c[0] } -> std::same_as< std::remove_cvref_t<T>& >;
        std::size(c);
    };

template <typename T, MutableIndexedContainerOf<T> C>
auto inc_multi_index(C& multi_index, const T& n) noexcept
    -> bool
{
    auto s = std::size(multi_index);
    for (size_t i=0; i<s; ++i)
    {
        auto& mi = multi_index[s-1-i];
        if (mi+1 < n)
        {
            ++mi;
            return true;
        }
        mi = 0;
    }
    if (s > 0)
    {
        multi_index[0] = n;
        std::fill(std::begin(multi_index)+1, std::end(multi_index), T{});
    }
    return false;
}

template <typename T, MutableIndexedContainerOf<T> C>
auto inc_multi_index_mono(C& multi_index, const T& n) noexcept
    -> bool
{
    auto s = std::size(multi_index);
    for (size_t i=0; i<s; ++i)
    {
        auto& mi = multi_index[s-1-i];
        if (mi+1 < n)
        {
            ++mi;
            for (size_t j=0; j<i; ++j)
                multi_index[s-1-j] = mi;
            return true;
        }
    }
    std::fill(std::begin(multi_index), std::end(multi_index), n);
    return false;
}

inline auto multi_index_mono_range_length(uint32_t n, uint32_t s)
    -> uint64_t
{ return s > 0 ?   binomial(common::Type<uint64_t>, n + s - 1, s) :   0; };

namespace detail {

template <typename T>
auto multi_index_mono_subrange_boundary(std::span<T> result,
                                        std::type_identity_t<T> n,
                                        uint64_t b,
                                        std::type_identity_t<T> n0) noexcept
    -> void
{
    auto s = result.size();

    if (s == 0)
        return;

    if (b == 0)
    {
        std::ranges::fill(result, n0);
        return;
    }

    if (s == 1)
    {
        result[0] = std::min(n0 + n - 1, n0 + static_cast<T>(b));
        return;
    }

    auto ncur = uint64_t{};
    for (T i=0; i<n; ++i)
    {
        result[0] = n0 + i;
        auto np = multi_index_mono_range_length(n-i, s-1);
        if (ncur + np <= b)
            ncur += np;
        else
        {
            multi_index_mono_subrange_boundary(
                result.subspan(1), n-i, b-ncur, n0+i);
            return;
        }
    }
    std::ranges::fill(result.subspan(1), result[0]);
}

} // namespace detail

template <typename T>
auto multi_index_mono_subrange_boundary(std::span<T> result,
                                        std::type_identity_t<T> n,
                                        uint32_t k,
                                        uint32_t p) noexcept
    -> void
{
    if (result.empty())
        return;

    if (k == 0)
    {
        std::fill(result.begin(), result.end(), T{});
        return;
    }

    if (k == p)
    {
        std::fill(result.begin(), result.end(), n);
        return;
    }

    auto b = multi_index_mono_range_length(n, result.size()) * k / p;

    ::gc_types::detail::multi_index_mono_subrange_boundary(result, n, b, 0);
}

template <typename T>
auto multi_index_mono_subrange_boundary(common::Type_Tag<T>,
                                        uint32_t s,
                                        std::type_identity_t<T> n,
                                        uint32_t k,
                                        uint32_t p) noexcept
    -> std::vector<T>
{
    auto result = std::vector<T>(s);
    multi_index_mono_subrange_boundary(std::span{result}, n, k, p);
    return result;
}

} // namespace gc_types
