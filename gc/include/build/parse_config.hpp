#pragma once

#include "build/config_fwd.hpp"

#include <yaml-cpp/node/node.h>


namespace build {

auto parse_config(const YAML::Node& node)
    -> Config;

} // namespace build
