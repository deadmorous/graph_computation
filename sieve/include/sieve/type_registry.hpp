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

#include "gc/type_registry.hpp"

namespace sieve {

auto populate_type_registry(gc::TypeRegistry&)
    -> void;

inline auto type_registry()
    -> gc::TypeRegistry
{
    auto result = gc::TypeRegistry{};
    populate_type_registry(result);
    return result;
}

} // namespace sieve
