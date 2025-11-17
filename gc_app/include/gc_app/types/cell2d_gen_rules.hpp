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

struct Cell2dGenRules_Overlay final
{
    std::string formula;
    Cell2dIndexRange range;

    auto operator==(const Cell2dGenRules_Overlay&) const noexcept
        -> bool = default;
};

struct Cell2dGenRules_Map final
{
    std::string formula;
    std::vector<Cell2dGenRules_Overlay> overlays;

    auto operator==(const Cell2dGenRules_Map&) const noexcept
        -> bool = default;
};

struct Cell2dGenRules final
{
    using Overlay = Cell2dGenRules_Overlay;
    using Map = Cell2dGenRules_Map;

    uint8_t state_count{255};
    int8_t min_state{-127};
    bool tor{true};
    bool count_self{false};
    Map map9;
    Map map6;
    Map map4;

    auto operator==(const Cell2dGenRules&) const noexcept -> bool = default;
};

GCLIB_STRUCT_TYPE(Cell2dGenRules_Overlay, formula, range);

GCLIB_STRUCT_TYPE(Cell2dGenRules_Map, formula, overlays);

GCLIB_STRUCT_TYPE(
    Cell2dGenRules,
    state_count, min_state, tor, count_self, map9, map6, map4);

} // namespace gc_app
