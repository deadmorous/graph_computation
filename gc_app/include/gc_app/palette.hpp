#pragma once

#include "gc_app/color.hpp"

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

constexpr inline auto fields_of(IndexedPalette& x)
    -> std::tuple<IndexedColorMap&, Color&>
{ return { x.color_map, x.overflow_color }; }

constexpr inline auto fields_of(const IndexedPalette& x)
    -> std::tuple<const IndexedColorMap&, const Color&>
{ return { x.color_map, x.overflow_color }; }

inline auto fields_of(IndexedPalette&& x) = delete;

constexpr inline auto tuple_tag_of(common::Type_Tag<IndexedPalette>)
    -> common::Type_Tag<std::tuple<IndexedColorMap, Color>>
{ return {}; }

constexpr inline auto field_names_of(common::Type_Tag<IndexedPalette>)
    -> std::array<std::string_view, 2>
{ return { "color_map", "overflow_color" }; }


auto map_color(const IndexedPalette& palette, Color value)
    -> Color;

auto average_color(const IndexedPalette& palette, bool with_overflow)
    -> Color;

} // namespace gc_app
