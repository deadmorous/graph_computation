/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024-2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "gc_visual/graph_output_visualizer.hpp"

#include "gc_visual/bitmap_view.hpp"
#include "gc_visual/graph_broker.hpp"
#include "gc_visual/qstr.hpp"

#include "gc/detail/parse_node_port.hpp"

#include <yaml-cpp/yaml.h>

#include <QBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QSlider>
#include <QTextEdit>


using namespace std::string_view_literals;

namespace {

auto make_image(GraphBroker* broker,
                const YAML::Node& item_node,
                GraphOutputVisualizer* parent)
    -> void
{
    parent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    auto* layout = new QVBoxLayout{};
    parent->setLayout(layout);

    // Resolve output port binding
    auto node_port_str = item_node["bind"].as<std::string>();
    auto output_port =
        gc::detail::parse_node_port(node_port_str,
                                    broker->named_nodes(),
                                    broker->node_indices(),
                                    gc::Output);

    auto* sub_layout = new QHBoxLayout{};
    layout->addLayout(sub_layout);

    auto* slider = new QSlider{ Qt::Horizontal };
    slider->setMinimum(1);
    slider->setMaximum(100);
    sub_layout->addWidget(slider);

    auto* save_button = new QPushButton("Sa&ve...");
    sub_layout->addWidget(save_button);

    auto* scroll_view = new QScrollArea{};

    auto* bitmap_view = new BitmapView{};
    scroll_view->setWidget(bitmap_view);

    layout->addWidget(scroll_view);

    QObject::connect(
        slider, &QSlider::valueChanged,
        bitmap_view,
        [=](int pos) { bitmap_view->set_scale(1. + (pos-1)/10.); });


    QObject::connect(
        save_button, &QPushButton::clicked,
        [bitmap_view, parent, last_saved_name=QString{}]() mutable {
            auto image = bitmap_view->qimage();
            if (image.isNull())
            {
                QMessageBox::critical(parent, {}, "Current image is empty");
                return;
            }

            auto file_name = QFileDialog::getSaveFileName(
                parent, "Save image", last_saved_name, "Images (*.png)");
            if (file_name.isEmpty())
                return;
            if (image.save(file_name))
                last_saved_name = file_name;
            else
                QMessageBox::critical(
                    parent, {}, "Failed to save file " + file_name);
        });

    auto on_output_updated = [=](gc::EdgeOutputEnd output)
    {
        if (output != output_port)
            return;

        bitmap_view->set_image(broker->get_port_value(output_port));
    };

    QObject::connect(
        broker, &GraphBroker::output_updated,
        bitmap_view, on_output_updated);

    on_output_updated(output_port);
}

auto make_text(GraphBroker* broker,
                const YAML::Node& item_node,
                GraphOutputVisualizer* parent)
    -> void
{
    parent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    auto* layout = new QVBoxLayout{};
    parent->setLayout(layout);

    // Resolve output port binding
    auto node_port_str = item_node["bind"].as<std::string>();
    auto output_port =
        gc::detail::parse_node_port(node_port_str,
                                    broker->named_nodes(),
                                    broker->node_indices(),
                                    gc::Output);

    auto view = new QTextEdit{};
    layout->addWidget(view);

    view->setReadOnly(true);
    view->setAcceptRichText(false);

    auto on_output_updated = [=](gc::EdgeOutputEnd output)
    {
        if (output != output_port)
            return;

        const auto& v = broker->get_port_value(output_port);

        constexpr auto max_lines = 1000ul;

        if (v.type()->aggregate_type() == gc::AggregateType::Vector)
        {
            view->clear();
            for (size_t i=0, n=std::min(v.size(), max_lines); i<n; ++i)
                view->append(format_qstr(i, '\t', v.get(gc::ValuePath{i})));

            if (v.size() > max_lines)
                view->append("...");
        }
        else
            view->setText(format_qstr(v));

        auto cursor = view->textCursor();
        cursor.setPosition(0);
        view->setTextCursor(cursor);
    };

    QObject::connect(
        broker, &GraphBroker::output_updated,
        view, on_output_updated );

    on_output_updated(output_port);
}

using GraphOutputVisualizerFactoryFunc =
    void(*)(GraphBroker*, const YAML::Node&, GraphOutputVisualizer*);

using GraphOutputVisualizerFactoryMap =
    std::unordered_map<std::string_view, GraphOutputVisualizerFactoryFunc>;



auto editor_factory_map() -> const GraphOutputVisualizerFactoryMap&
{
    static auto result = GraphOutputVisualizerFactoryMap{
        { "image"sv, make_image },
        { "text"sv, make_text },
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
    editor_factory_map().at(type)(broker, item_node, this);
}

auto GraphOutputVisualizer::supports_type(const std::string& type) -> bool
{
    return editor_factory_map().contains(type);
}
