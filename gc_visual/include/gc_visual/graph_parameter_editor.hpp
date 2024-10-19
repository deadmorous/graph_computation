#pragma once

#include "gc_visual/graph_broker.hpp"

#include "yaml-cpp/node/node.h"

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
    gc::Node* node_;
    size_t index_{0};
    gc::ValuePath path_;
    std::shared_ptr<QWidget> res_;
};
