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

#include <QWidget>

#include <memory>

class EvolutionController final :
    public QWidget
{
    Q_OBJECT

public:
    EvolutionController(
        const std::string& type,
        GraphBroker* broker,
        QWidget* parent = nullptr);
};
