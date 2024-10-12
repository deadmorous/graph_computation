#pragma once

#include "gc/type_registry.hpp"

namespace gc_app {

auto populate_type_registry(gc::TypeRegistry&)
    -> void;

inline auto type_registry()
    -> gc::TypeRegistry
{
    auto result = gc::TypeRegistry{};
    populate_type_registry(result);
    return result;
}

} // namespace gc_app
