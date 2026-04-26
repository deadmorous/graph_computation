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

#include "mpk/mix/value/value.hpp"
#include "mpk/mix/serial/yaml/parse_value.hpp"

namespace gc::yaml {

template <typename T>
auto parse_typed_value(const YAML::Node& node,
                 const TypeRegistry& type_registry = {},
                 mpk::mix::Type_Tag<T> = {})
    -> T
{
    const auto* type = type_of<T>();
    auto value = mpk::mix::serial::yaml::parse_value(node, type, type_registry);
    return value.template as<T>();
}

} // namespace gc::yaml
