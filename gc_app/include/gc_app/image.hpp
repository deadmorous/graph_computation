#pragma once

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

// ---

inline auto rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xff)
    -> uint32_t
{
    return
          (uint32_t{a} << 24)
        | (uint32_t{r} << 16)
        | (uint32_t{g} <<  8)
        | (uint32_t{b});
}

inline auto rgba(uint32_t rgb, uint8_t a = 0xff)
    -> uint32_t
{ return (uint32_t{a} << 24) | (rgb & 0xffffff); }

// ---

struct Image final
{
    UintSize                size;
    std::vector<uint32_t>   data;
};

} // namespace gc_app

GC_REGISTER_CUSTOM_TYPE(gc_app::Image, 1);
