#include "gc_visual/mainwindow.hpp"

#include "gc_visual/graph_broker.hpp"
#include "gc_visual/graph_output_visualizer.hpp"
#include "gc_visual/graph_parameter_editor.hpp"

#include "gc_app/node_registry.hpp"
#include "gc_app/type_registry.hpp"

#include "gc/graph_computation.hpp"
#include "gc/yaml/parse_graph.hpp"

#include "common/throw.hpp"

#include "yaml-cpp/yaml.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QDebug>

namespace {

auto load_config(const gc_visual::ConfigSpecification& spec)
    -> YAML::Node
{
    switch (spec.type)
    {
    case gc_visual::ConfigSpecificationType::Content:
        return YAML::Load(spec.spec);
    case gc_visual::ConfigSpecificationType::FileName:
        return YAML::LoadFile(spec.spec);
    }
    __builtin_unreachable();
}

auto parse_nested_layout(QBoxLayout* layout,
                         GraphBroker* broker,
                         const YAML::Node& layout_node,
                         QLayoutItem* parent_item)
    -> void;

auto parse_layout_item(GraphBroker* broker,
                       const YAML::Node& item_node,
                       QLayoutItem* parent_item)
    -> void;

auto parse_nested_layout(QBoxLayout* layout,
                         GraphBroker* broker,
                         const YAML::Node& layout_node,
                         QLayoutItem* parent_item)
    -> void
{
    if (auto* parent_layout = qobject_cast<QBoxLayout*>(parent_item->layout()))
        parent_layout->addLayout(layout);
    else if (auto* parent_widget = parent_item->widget())
        parent_widget->setLayout(layout);
    else
        assert(false);

    for (const auto& item_node : layout_node["items"])
        parse_layout_item(broker, item_node, layout);
}

auto parse_layout_item(GraphBroker* broker,
                       const YAML::Node& item_node,
                       QLayoutItem* parent_item)
    -> void
{
    auto type = item_node["type"].as<std::string>();
    if (type == "horizontal_layout")
        parse_nested_layout(new QHBoxLayout{}, broker, item_node, parent_item);
    else if (type == "vertical_layout")
        parse_nested_layout(new QVBoxLayout{}, broker, item_node, parent_item);
    else
    {
        QWidget* widget = nullptr;

        if (type == "spin")
            widget = new GraphParameterEditor(type, broker, item_node);
        else if (type == "image")
            widget = new GraphOutputVisualizer(type, broker, item_node);
        else
            common::throw_("Unknown layout item type '", type, "'");

        if (auto parent_layout = parent_item->layout())
            parent_layout->addWidget(widget);
        else if (auto parent_widget = parent_item->widget())
            widget->setParent(parent_widget);
        else
            assert(false);
    }
}

auto parse_layout(const YAML::Node& config,
                  gc::Computation& computation,
                  const gc::detail::NamedNodes& node_map,
                  QWidget* parent = nullptr)
    -> QWidget*
{
    auto host = new QWidget(parent);
    auto broker = new GraphBroker(computation, node_map, host);

    auto host_layout_item = QWidgetItem{host};
    parse_layout_item(broker, config, &host_layout_item);

    return host;
}

} // anonymous namespace


MainWindow::MainWindow(const gc_visual::ConfigSpecification& spec,
                       QWidget *parent)
    : QMainWindow(parent)
{
    // Parse YAML config
    auto config = load_config(spec);

    // Initialize node registry and type registry
    auto node_registry = gc::node_registry();
    gc_app::populate_node_registry(node_registry);
    auto type_registry = gc::type_registry();
    gc_app::populate_type_registry(type_registry);

    // Parse graph from the node object.
    auto graph_config = config["graph"];
    auto [g, node_map] =
        gc::yaml::parse_graph(graph_config, node_registry, type_registry);

    // Compile and compute the graph
    computation_ = computation(std::move(g));
    compute(computation_);

    // Create visual layout
    auto layout = config["layout"];
    auto central_widget = parse_layout(layout, computation_, node_map);
    setCentralWidget(central_widget);
}
