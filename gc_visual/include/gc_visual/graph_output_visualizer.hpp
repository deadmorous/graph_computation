#pragma once

#include "gc/edge.hpp"

#include "yaml-cpp/node/node.h"

#include <QWidget>

class BitmapView;
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

public slots:
    auto on_output_updated(gc::EdgeEnd output)
        -> void;

private:
    GraphBroker* broker_;
    BitmapView* view_;
    gc::EdgeEnd output_port_;
};
