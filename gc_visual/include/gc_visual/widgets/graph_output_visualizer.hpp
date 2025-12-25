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

#include "gc/edge.hpp"

#include <yaml-cpp/node/node.h>

#include <QWidget>

class GraphBroker;

class GraphOutputVisualizer final :
    public QWidget
{
    Q_OBJECT

public:
    GraphOutputVisualizer(
        const std::string& type,
        GraphBroker* broker,
        const YAML::Node& item_node,
        QWidget* parent = nullptr);

    static auto supports_type(const std::string& type) -> bool;
};
