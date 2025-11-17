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

#include "gc_app/types/cell2d_gen_range.hpp"

#include "common/struct_type_macro.hpp"

#include <cstdint>
#include <string>
#include <vector>


namespace gc_app {

template <typename T>
struct Cell2dGenCmap_Rgb final
{
    T r;
    T g;
    T b;

    auto operator==(const Cell2dGenCmap_Rgb&) const noexcept
        -> bool = default;
};

using Cell2dGenCmap_RgbFormula = Cell2dGenCmap_Rgb<std::string>;

struct Cell2dGenCmap_Overlay final
{
    Cell2dGenCmap_RgbFormula formula;
    Cell2dIndexRange range;

    auto operator==(const Cell2dGenCmap_Overlay&) const noexcept
        -> bool = default;
};

struct Cell2dGenCmap final
{
    using Overlay = Cell2dGenCmap_Overlay;

    int state_count{255};
    Cell2dGenCmap_RgbFormula formula;
    std::vector<Cell2dGenCmap_Overlay> overlays;

    auto operator==(const Cell2dGenCmap&) const noexcept -> bool = default;
};

GCLIB_STRUCT_TYPE(
    Cell2dGenCmap_RgbFormula, r, g, b);

GCLIB_STRUCT_TYPE(
    Cell2dGenCmap_Overlay, formula, range);

GCLIB_STRUCT_TYPE(
    Cell2dGenCmap,
    state_count, formula, overlays);

} // namespace gc_app
