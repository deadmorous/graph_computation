#pragma once

#include "gc_visual/graph_broker.hpp"

#include <yaml-cpp/node/node.h>

#include <QWidget>

#include <memory>

class GraphParameterEditor final :
    public QWidget
{
    Q_OBJECT

public:
    GraphParameterEditor(
        const std::string& type,
        GraphBroker* broker,
        const YAML::Node& item_node,
        QWidget* parent = nullptr);

private:
    std::shared_ptr<QWidget> res_;
};
