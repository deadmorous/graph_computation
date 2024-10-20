#pragma once

#include "gc_app/color.hpp"
#include "gc_app/types.hpp"

namespace gc_app {

template <typename T>
struct Size final
{
    T width;
    T height;
};

template <typename T>
constexpr inline auto fields_of(Size<T>& x)
    -> std::tuple<T&, T&>
{ return { x.width, x.height }; }

template <typename T>
constexpr inline auto fields_of(const Size<T>& x)
    -> std::tuple<const T&, const T&>
{ return { x.width, x.height }; }

template <typename T>
inline auto fields_of(Size<T>&& x) = delete;

template <typename T>
constexpr inline auto tuple_tag_of(common::Type_Tag<Size<T>>)
    -> common::Type_Tag<std::tuple<T, T>>
{ return {}; }

template <typename T>
constexpr inline auto field_names_of(common::Type_Tag<Size<T>>)
    -> std::array<std::string_view, 2>
{ return { "width", "height" }; }

using UintSize = Size<Uint>;

struct Image final
{
    UintSize                size;
    std::vector<Color>      data;
};

} // namespace gc_app

GC_REGISTER_CUSTOM_TYPE(gc_app::Image, 1);
