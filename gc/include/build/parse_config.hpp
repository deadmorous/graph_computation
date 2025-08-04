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
