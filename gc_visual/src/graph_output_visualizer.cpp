#include "gc_visual/graph_output_visualizer.hpp"

#include "gc_visual/bitmap_view.hpp"
#include "gc_visual/graph_broker.hpp"

#include "gc/detail/parse_node_port.hpp"

#include <QSlider>
#include <QVBoxLayout>


GraphOutputVisualizer::GraphOutputVisualizer(const std::string& type,
                                             GraphBroker* broker,
                                             const YAML::Node& item_node,
                                             QWidget* parent) :
    QWidget{ parent },
    broker_{ broker }
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    auto* layout = new QVBoxLayout{};
    setLayout(layout);

    assert(type == "image");

    // Resolve output port binding
    auto node_port_str = item_node["bind"].as<std::string>();
    output_port_ =
        gc::detail::parse_node_port(node_port_str,
                                    broker->named_nodes(),
                                    gc::Output);

    auto slider = new QSlider{ Qt::Horizontal };
    slider->setMinimum(1);
    slider->setMaximum(100);
    layout->addWidget(slider);

    view_ = new BitmapView{};
    layout->addWidget(view_);

    connect(slider, &QSlider::valueChanged,
            [this](int pos) { view_->set_scale(1. + (pos-1)/10.); });

    connect(broker, &GraphBroker::output_updated,
            this, &GraphOutputVisualizer::on_output_updated);

    on_output_updated(output_port_);
}

auto GraphOutputVisualizer::on_output_updated(gc::EdgeEnd output)
    -> void
{
    if (output != output_port_)
        return;

    view_->set_image(broker_->get_port_value(output_port_, gc::Output));
}
