#pragma once

#include "gc_app/color.hpp"

#include "common/struct_type_macro.hpp"
#include "common/type.hpp"

#include <array>
#include <cstdint>
#include <string_view>
#include <tuple>
#include <vector>

namespace gc_app {

using IndexedColorMap =
    std::vector<Color>;

struct IndexedPalette
{
    IndexedColorMap color_map;
    Color overflow_color;
};

GCLIB_STRUCT_TYPE(IndexedPalette, color_map, overflow_color);


auto map_color(const IndexedPalette& palette, uint32_t value)
    -> Color;

auto average_color(const IndexedPalette& palette, bool with_overflow)
    -> Color;

} // namespace gc_app
