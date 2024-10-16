#pragma once

#include "gc_visual/graph_broker.hpp"

#include "yaml-cpp/node/node.h"

#include <QWidget>

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

public slots:
    auto on_output_updated(gc::EdgeEnd output)
        -> void;
};
