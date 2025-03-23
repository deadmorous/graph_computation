/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "build/config.hpp"
#include "build/parse_config.hpp"

#include "gc/value.hpp"
#include "gc/yaml/parse_value.hpp"


namespace build {

auto parse_config(const YAML::Node& node)
    -> Config
{
    const auto* type = gc::Type::of<Config>();
    auto value = gc::yaml::parse_value(node, type, {});
    return value.as<Config>();
}

} // namespace build
