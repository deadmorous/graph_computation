#include "gc_visual/graph_output_visualizer.hpp"

GraphOutputVisualizer::GraphOutputVisualizer(const std::string& type,
                                             GraphBroker* broker,
                                             const YAML::Node& item_node,
                                             QWidget* parent) :
    QWidget{ parent }
{
    connect(broker, &GraphBroker::output_updated,
            this, &GraphOutputVisualizer::on_output_updated);
}

auto GraphOutputVisualizer::on_output_updated(gc::EdgeEnd output)
    -> void
{
    // TODO
    // ...
    update();
}
