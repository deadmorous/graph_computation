#pragma once

#include "gc_app/color.hpp"
#include "gc_app/types.hpp"

#include "gc/struct_type_macro.hpp"

namespace gc_app {

template <typename T>
struct Size final
{
    T width;
    T height;
};

using UintSize = Size<Uint>;

GCLIB_STRUCT_TYPE(UintSize, width, height);


struct Image final
{
    UintSize                size;
    std::vector<Color>      data;
};

} // namespace gc_app

GC_REGISTER_CUSTOM_TYPE(gc_app::Image, 1);
