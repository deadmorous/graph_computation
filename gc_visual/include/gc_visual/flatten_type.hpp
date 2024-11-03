#pragma once

#include "gc/type_fwd.hpp"
#include "gc/value_path.hpp"

#include <vector>


namespace gc_visual {

struct TypeComponent final
{
    const gc::Type* type;
    gc::ValuePath path;
};

using TypeComponentVec = std::vector<TypeComponent>;

auto flatten_type(const gc::Type* t)
    -> std::vector<TypeComponent>;

} // namespace gc_visual