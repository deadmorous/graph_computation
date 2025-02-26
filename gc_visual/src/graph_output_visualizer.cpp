#include "gc_visual/graph_output_visualizer.hpp"

#include "gc_visual/bitmap_view.hpp"
#include "gc_visual/graph_broker.hpp"
#include "gc_visual/qstr.hpp"

#include "gc/detail/parse_node_port.hpp"

#include <yaml-cpp/yaml.h>

#include <QSlider>
#include <QTextEdit>
#include <QVBoxLayout>


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

    auto slider = new QSlider{ Qt::Horizontal };
    slider->setMinimum(1);
    slider->setMaximum(100);
    layout->addWidget(slider);

    auto view = new BitmapView{};
    layout->addWidget(view);

    QObject::connect(
        slider, &QSlider::valueChanged,
        view,
        [=](int pos) { view->set_scale(1. + (pos-1)/10.); });

    auto on_output_updated = [=](gc::EdgeOutputEnd output)
    {
        if (output != output_port)
            return;

        view->set_image(broker->get_port_value(output_port));
    };

    QObject::connect(
        broker, &GraphBroker::output_updated,
        view, on_output_updated );

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

} // anonymous namespace


GraphOutputVisualizer::GraphOutputVisualizer(const std::string& type,
                                             GraphBroker* broker,
                                             const YAML::Node& item_node,
                                             QWidget* parent) :
    QWidget{ parent }
{
    if(type == "image")
        make_image(broker, item_node, this);
    else if(type == "text")
        make_text(broker, item_node, this);
    else
        common::throw_("Unknown graph output visualizer type '", type, '\'');
}
