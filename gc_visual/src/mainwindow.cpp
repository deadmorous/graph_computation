#include "gc_visual/mainwindow.hpp"

#include "gc_visual/parse_layout.hpp"

#include "gc_app/node_registry.hpp"
#include "gc_app/type_registry.hpp"

#include "gc/yaml/parse_graph.hpp"

#include "yaml-cpp/yaml.h"


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

} // anonymous namespace


MainWindow::MainWindow(const gc_visual::ConfigSpecification& spec,
                       QWidget *parent)
    : QMainWindow(parent)
{
    resize(800, 600);
    load(spec);
}

auto MainWindow::load(const gc_visual::ConfigSpecification& spec)
    -> void
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
