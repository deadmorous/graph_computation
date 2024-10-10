#pragma once

#include "gc/graph.hpp"

#include "yaml-cpp/node/node.h"

namespace gc {

auto load_graph(const YAML::Node&)
    -> Graph;

} // namespace gc
