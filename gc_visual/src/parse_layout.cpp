/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024-2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#include "gc_visual/parse_layout.hpp"

#include "gc_visual/evolution_controller.hpp"
#include "gc_visual/graph_broker.hpp"
#include "gc_visual/graph_output_visualizer.hpp"
#include "gc_visual/graph_parameter_editor.hpp"
#include "gc_visual/invalidate_button.hpp"

#include "common/throw.hpp"

#include <yaml-cpp/yaml.h>

#include <QHBoxLayout>
#include <QVBoxLayout>

namespace {

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
    else if (type == "stretch")
    {
        auto layout = qobject_cast<QBoxLayout*>(parent_item->layout());
        if (!layout)
            common::throw_("Cannot insert a stretch into a widget");
        int value = 0;
        if (auto value_node = item_node["value"])
            value = value_node.as<int>();
        layout->addStretch(value);
    }
    else
    {
        QWidget* widget = nullptr;
        int stretch = 0;

        if (GraphParameterEditor::supports_type(type))
            widget = new GraphParameterEditor(type, broker, item_node);
        else if (GraphOutputVisualizer::supports_type(type))
        {
            widget = new GraphOutputVisualizer(type, broker, item_node);
            stretch = 1;
        }
        else if (type == "evolution")
            widget = new EvolutionController(type, broker);
        else if (type == "invalidate")
            widget = new InvalidateButton(broker, item_node);
        else
            common::throw_("Unknown layout item type '", type, "'");

        if (auto* parent_layout = parent_item->layout())
        {
            if (auto* box_layout = qobject_cast<QBoxLayout*>(parent_layout))
                box_layout->addWidget(widget, stretch);
            else
                parent_layout->addWidget(widget);
        }
        else if (auto* parent_widget = parent_item->widget())
            widget->setParent(parent_widget);
        else
            assert(false);
    }
}

} // anonymous namespace


auto parse_layout(const YAML::Node& config,
                  ComputationThread& computation_thread,
                  const gc::detail::NamedComputationNodes& node_map,
                  const std::vector<std::string>& input_names,
                  QWidget* parent)
    -> std::pair<QWidget*, GraphBroker*>
{
    auto host = new QWidget(parent);
    auto broker =
        new GraphBroker{ computation_thread, node_map, input_names, host };

    auto host_layout_item = QWidgetItem{host};
    parse_layout_item(broker, config, &host_layout_item);

    return { host, broker };
}
