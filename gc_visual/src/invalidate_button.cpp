/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "gc_visual/invalidate_button.hpp"

#include <yaml-cpp/yaml.h>

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QTimer>


InvalidateButton::InvalidateButton(
    GraphBroker* broker,
    const YAML::Node& item_node,
    QWidget* parent) :
    QPushButton{parent}
{
    auto binding =
        gc_visual::parse_param_binding(broker->binding_resolver(), item_node);
    auto label_text = QString{};
    if (auto label_node = item_node["label"])
        label_text = QString::fromUtf8(label_node.as<std::string>());
    else
        label_text = QString::fromUtf8("-> " + binding.io_name);
    setText(label_text);
    connect(this, &QPushButton::clicked,
            [broker, binding]{
                broker->invalidate_input(binding.param_spec);
            });
}
