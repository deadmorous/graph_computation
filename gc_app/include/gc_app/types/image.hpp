/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024-2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "gc_app/types/color.hpp"
#include "gc_app/types/uint.hpp"

#include "common/struct_type_macro.hpp"


namespace gc_app {

template <typename T>
struct Size final
{
    T width;
    T height;

    auto operator==(const Size&) const noexcept -> bool = default;
};

using UintSize = Size<Uint>;

GCLIB_STRUCT_TYPE(UintSize, width, height);


using ColorVec =
    std::vector<Color>;

template <typename Pixel>
struct Image final
{
    UintSize size;
    std::vector<Pixel> data;
};

using ColorImage = Image<Color>;
using I8Image = Image<int8_t>;
using U8Image = Image<uint8_t>;
using I16Image = Image<int16_t>;
using U16Image = Image<uint16_t>;
using I32Image = Image<int32_t>;
using U32Image = Image<uint32_t>;

} // namespace gc_app

GC_REGISTER_CUSTOM_TYPE(gc_app::ColorImage, 1);
GC_REGISTER_CUSTOM_TYPE(gc_app::I8Image, 2);
GC_REGISTER_CUSTOM_TYPE(gc_app::U8Image, 3);
GC_REGISTER_CUSTOM_TYPE(gc_app::I16Image, 4);
GC_REGISTER_CUSTOM_TYPE(gc_app::U16Image, 5);
GC_REGISTER_CUSTOM_TYPE(gc_app::I32Image, 6);
GC_REGISTER_CUSTOM_TYPE(gc_app::U32Image, 7);
