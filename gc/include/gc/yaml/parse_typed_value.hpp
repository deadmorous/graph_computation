/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "gc/value.hpp"
#include "gc/yaml/parse_value.hpp"

namespace gc::yaml {

template <typename T>
auto parse_typed_value(const YAML::Node& node,
                 const TypeRegistry& type_registry = {},
                 common::Type_Tag<T> = {})
    -> T
{
    const auto* type = type_of<T>();
    auto value = gc::yaml::parse_value(node, type, type_registry);
    return value.template as<T>();
}

} // namespace gc::yaml
