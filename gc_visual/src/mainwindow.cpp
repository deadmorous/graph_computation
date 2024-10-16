#include "gc_visual/mainwindow.hpp"

#include "gc_visual/bitmap_view.hpp"
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
                         const YAML::Node& layout_node)
    -> void;

auto parse_layout_item(GraphBroker* broker,
                       const YAML::Node& item_node,
                       QWidget* item_parent)
    -> QWidget*;

auto parse_nested_layout(QBoxLayout* layout,
                         GraphBroker* broker,
                         const YAML::Node& layout_node)
    -> void
{
    for (const auto& item_node : layout_node["items"])
        layout->addWidget(
            parse_layout_item(broker, item_node, nullptr));
}

auto parse_layout_item(GraphBroker* broker,
                       const YAML::Node& item_node,
                       QWidget* item_parent)
    -> QWidget*
{
    auto type = item_node["type"].as<std::string>();
    if (type == "horizontal_layout")
    {
        parse_nested_layout(new QHBoxLayout(item_parent), broker, item_node);
        return item_parent;
    }
    else if (type == "vertical_layout")
    {
        parse_nested_layout(new QVBoxLayout(item_parent), broker, item_node);
        return item_parent;
    }
    else if (type == "spin")
        return new GraphParameterEditor(type, broker, item_node, item_parent);
    else if (type == "image")
        return new GraphOutputVisualizer(type, broker, item_node, item_parent);
    else
        common::throw_("Unknown layout item type '", type, "'");
}

auto parse_layout(const YAML::Node& config,
                  gc::Graph& g,
                  const gc::detail::NamedNodes& node_map,
                  QWidget* parent = nullptr)
    -> QWidget*
{
    auto host = new QWidget(parent);
    auto broker = new GraphBroker(g, node_map, host);

    parse_layout_item(broker, config, host);

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
    auto instr = compile(g);
    auto result = gc::ComputationResult{};
    compute(result, g, instr.get());

    // Compute node index map
    auto node_index_map = std::unordered_map<const gc::Node*, uint32_t>{};
    for (uint32_t index=0; const auto& node: g.nodes)
        node_index_map[node.get()] = index++;

    auto node_and_index =
        [&](const std::string& node_name) -> std::pair<gc::Node*, uint32_t>
    {
        auto it = node_map.find(node_name);
        if (it == node_map.end())
            common::throw_("Graph node '", node_name, "' is not found");
        return { it->second, node_index_map.at(it->second) };
    };

    // Create visual layout
    auto layout = config["layout"];

    // TODO

    const auto& image = group(result.outputs,7)[0].as<gc_app::Image>();

    setCentralWidget(new BitmapView{image});
}
