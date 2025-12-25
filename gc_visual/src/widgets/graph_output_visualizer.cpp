/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024-2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "gc_visual/widgets/graph_output_visualizer.hpp"
#include "gc_visual/visualizers/visualizer_widget.hpp"

#include "gc_visual/graph_broker.hpp"
#include "gc_visual/qstr.hpp"
#include "gc_visual/visualizers/image_visualizer.hpp"
#include "gc_visual/visualizers/text_visualizer.hpp"

#include "gc/detail/parse_node_port.hpp"

#include "common/func_ref.hpp"
#include "common/throw.hpp"

#include <QBoxLayout>
#include <QLabel>


using namespace std::string_view_literals;

namespace {

using VisualizerFactoryFunc =
    common::FuncRef<VisualizerWidget*(
        const std::string&,
        const gc::EdgeOutputEnd&,
        GraphBroker*,
        const YAML::Node&)>;

template <std::derived_from<VisualizerWidget> Visualizer>
auto visualizer_factory() -> VisualizerFactoryFunc
{
    static constexpr auto impl = [](const std::string& visualizer_type,
                                    const gc::EdgeOutputEnd& port,
                                    GraphBroker* broker,
                                    const YAML::Node& item_node)
    {
        auto value = broker->get_port_value(port);

        if (auto type_check_result = Visualizer::check_type(value.type());
            !type_check_result.ok)
        {
            common::throw_(
                "Invalid binding: '", visualizer_type,
                "' can only bind to ",
                type_check_result.expected_type_description,
                ", whereas the output ", common::format(port),
                " is of type ", value.type());
        }

        return new Visualizer(broker, item_node);
    };
    return &impl;
}

using VisualizerFactoryMap =
    std::unordered_map<std::string_view, VisualizerFactoryFunc>;

auto visualizer_factory_map() -> const VisualizerFactoryMap&
{
    static auto result = VisualizerFactoryMap{
        { "image"sv, visualizer_factory<ImageVisualizer>() },
        { "text"sv, visualizer_factory<TextVisualizer>() },
    };

    return result;
}

} // anonymous namespace


GraphOutputVisualizer::GraphOutputVisualizer(const std::string& type,
                                             GraphBroker* broker,
                                             const YAML::Node& item_node,
                                             QWidget* parent) :
    QWidget{ parent }
{
    // Resolve output port binding
    auto node_port_str = item_node["bind"].as<std::string>();
    auto port =
        gc::detail::parse_node_port(node_port_str,
                                    broker->named_nodes(),
                                    broker->node_indices(),
                                    gc::Output);

    auto* visualizer = visualizer_factory_map().at(type)(
        type, port, broker, item_node);

    auto layout = new QVBoxLayout{};
    setLayout(layout);
    auto* label = new QLabel(qstr(node_port_str));
    label->setBuddy(visualizer);
    layout->addWidget(label);
    layout->addWidget(visualizer);

    auto on_output_updated = [=](gc::EdgeOutputEnd output)
    {
        if (output != port)
            return;

        visualizer->set_value(broker->get_port_value(port));
    };

    QObject::connect(
        broker, &GraphBroker::output_updated,
        visualizer, on_output_updated);

    on_output_updated(port);
}

auto GraphOutputVisualizer::supports_type(const std::string& type) -> bool
{
    return visualizer_factory_map().contains(type);
}
