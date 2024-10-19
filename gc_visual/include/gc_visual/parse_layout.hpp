#pragma once

#include "gc/detail/named_nodes.hpp"
#include "gc/graph_computation.hpp"

#include "yaml-cpp/node/node.h"

#include <QWidget>


auto parse_layout(const YAML::Node& config,
                  gc::Computation& computation,
                  const gc::detail::NamedNodes& node_map,
                  QWidget* parent = nullptr)
    -> QWidget*;
