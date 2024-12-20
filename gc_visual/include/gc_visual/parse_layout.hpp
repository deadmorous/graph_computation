#pragma once

#include "gc_visual/computation_thread.hpp"

#include "gc/detail/named_nodes.hpp"

#include "yaml-cpp/node/node.h"

#include <QWidget>


auto parse_layout(const YAML::Node& config,
                  ComputationThread& computation_thread,
                  const gc::detail::NamedNodes& node_map,
                  const std::vector<std::string>& input_names,
                  QWidget* parent = nullptr)
    -> QWidget*;
