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

#include "gc_visual/graph_broker_fwd.hpp"
#include "gc_visual/visualizers/visualizer_widget.hpp"

#include <QWidget>

class TextVisualizer final :
    public VisualizerWidget
{
    Q_OBJECT

public:
    explicit TextVisualizer(GraphBroker* broker,
                            const YAML::Node&,
                            QWidget* parent = nullptr);

    ~TextVisualizer();

    static auto check_type(const gc::Type*) -> TypeCheckResult;

public slots:
    void set_value(const gc::Value& v) override;

public:
    struct Storage;

private:
    std::unique_ptr<Storage> storage_;
};
