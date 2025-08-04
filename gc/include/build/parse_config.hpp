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

#include "build/config_fwd.hpp"
#include "build/lib_config_fwd.hpp"

#include <yaml-cpp/node/node.h>


namespace build {

auto parse_config(const YAML::Node& node)
    -> Config;

auto parse_lib_config(const YAML::Node& node)
    -> LibConfig;

} // namespace build
