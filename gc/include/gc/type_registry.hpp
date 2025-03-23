/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "gc/type_fwd.hpp"

#include "common/value_registry.hpp"

namespace gc {

using TypeRegistry =
    common::ValueRegistry<const Type*>;

auto populate_gc_type_registry(TypeRegistry&)
    -> void;

inline auto type_registry()
    -> TypeRegistry
{
    auto result = TypeRegistry{};
    populate_gc_type_registry(result);
    return result;
}

} // namespace gc
