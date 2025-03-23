/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024-2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "gc_visual/computation_thread.hpp"

#include "gc/detail/named_computation_nodes.hpp"

#include <yaml-cpp/node/node.h>

#include <QWidget>


auto parse_layout(const YAML::Node& config,
                  ComputationThread& computation_thread,
                  const gc::detail::NamedComputationNodes& node_map,
                  const std::vector<std::string>& input_names,
                  QWidget* parent = nullptr)
    -> QWidget*;
