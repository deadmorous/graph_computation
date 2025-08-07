#pragma once

#include "agc_app_rt/types/grid_2d_spec.hpp"
#include "agc_app_rt/types/point_2d.hpp"

#include "common/throw.hpp"

#include <cmath>


namespace agc_app_rt {

inline auto grid_2d_size(const Grid2dSpec& spec) noexcept
    -> Point2d_u32
{
    auto size_along = [&](size_t axis) -> uint32_t
    { return std::lround(spec.rect[axis].length() / spec.resolution[axis]); };
    return { size_along(0), size_along(1) };
}

struct Grid2dIter final
{
    Point2d_u32 size;
    Point2d_u32 index;
    Point2d_d origin;
    Point2d_d v;
    Point2d_d h;
};

auto grid_2d_init_iter(const Grid2dSpec& spec)
    -> Grid2dIter
{
    auto size = grid_2d_size(spec);
    if (size[0] == 0 || size[1] == 0)
        common::throw_<std::invalid_argument>("Empty grid");

    auto origin = Point2d_d{ spec.rect[0].begin, spec.rect[1].begin };

    return {
        .size = size,
        .index = { 0u, 0u },
        .origin = origin,
        .v = origin,
        .h = spec.resolution };
}

auto grid_2d_next_iter(Grid2dIter& iter) noexcept
    -> bool
{
    if (iter.index[0] + 1 < iter.size[0]) [[ likely ]]
    {
        ++iter.index[0];
        iter.v[0] += iter.h[0];
        return true;
    }

    if (iter.index[1] + 1 < iter.size[1]) [[ likely ]]
    {
        iter.index[0] = 0;
        ++iter.index[1];
        iter.v[0] = iter.origin[0];
        iter.v[1] += iter.h[1];
        return true;
    }

    return false;
}

auto grid_2d_deref_iter(const Grid2dIter& iter) noexcept
    -> Point2d_d
{ return iter.v; }

} // namespace agc_app_rt
