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

#include "gc_visual/graph_broker.hpp"

#include <QPushButton>

class InvalidateButton final :
    public QPushButton
{
    Q_OBJECT

public:
    explicit InvalidateButton(
        GraphBroker* broker,
        const YAML::Node& item_node,
        QWidget* parent = nullptr);
};
