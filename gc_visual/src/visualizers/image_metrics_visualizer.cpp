/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "gc_visual/visualizers/image_metrics_visualizer.hpp"

#include "gc_visual/graph_broker.hpp"
#include "gc_visual/widgets/image_metrics_view.hpp"

#include "sieve/types/image_metrics.hpp"

#include "common/throw.hpp"

#include <yaml-cpp/yaml.h>

#include <magic_enum/magic_enum.hpp>

#include <QBoxLayout>
#include <QComboBox>
#include <QTextEdit>

struct ImageMetricsVisualizer::Storage
{
    ImageMetricsView* view{};
    QVector<double> x;
    QVector<double> y;
};

ImageMetricsVisualizer::ImageMetricsVisualizer(GraphBroker* broker,
                               const YAML::Node& item_node,
                               QWidget* parent):
    VisualizerWidget{ parent },
    storage_{std::make_unique<Storage>()}
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    auto* layout = new QVBoxLayout{};
    setLayout(layout);

    size_t lookback = 1000;
    if (auto lookback_node = item_node["lookback"]; lookback_node.IsDefined())
        lookback = lookback_node.as<size_t>();

    auto* sub_layout = new QHBoxLayout{};
    layout->addLayout(sub_layout);
    auto* type_list = new QComboBox{};
    sub_layout->addWidget(type_list);
    type_list->addItem(
        "State histogram",
        QVariant::fromValue(ImageMetricsView::Type::StateHistogram));
    type_list->addItem("Edge histogram",
        QVariant::fromValue(ImageMetricsView::Type::EdgeHistogram));

    auto* view = storage_->view = new ImageMetricsView{ lookback };
    layout->addWidget(view);

    if (auto palette_node = item_node["palette"]; palette_node.IsDefined())
    {
        auto binding_str = palette_node.as<std::string>();
        const auto& binding_resolver = broker->binding_resolver();
        auto palette_binding =
            gc_visual::parse_param_binding(binding_resolver, binding_str);

        auto value = broker->get_parameter(palette_binding.param_spec);
        view->set_palette(value.as<gc_app::IndexedPalette>());

        auto param_spec = palette_binding.param_spec;
        if (holds_alternative<gc::NodeOutputSpec>(param_spec.io))
        {
            auto output = get<gc::NodeOutputSpec>(param_spec.io).output;
            QObject::connect(
                broker, &GraphBroker::output_updated,
                [output, broker, param_spec, view](gc::EdgeOutputEnd updated)
                {
                    if (output != updated)
                        return;
                    auto updated_value = broker->get_parameter(param_spec);
                    view->set_palette(updated_value.as<gc_app::IndexedPalette>());
                });
        }
    }

    QObject::connect(
        type_list, &QComboBox::currentIndexChanged,
        view, [type_list, view](int index) {
            auto type =
                type_list->currentData().value<ImageMetricsView::Type>();
            view->set_type(type);
        });

    if (auto type_node = item_node["metric"]; type_node.IsDefined())
    {
        auto type_str = type_node.as<std::string>();
        auto type = magic_enum::enum_cast<ImageMetricsView::Type>(type_str);
        if (!type)
            common::throw_(
                "Invalid metric type: expected one of ",
                common::format_seq(
                    magic_enum::enum_names<ImageMetricsView::Type>(), ", "),
                ", got ", type_str);
        type_list->setCurrentIndex(static_cast<int>(*type));
    }

}

ImageMetricsVisualizer::~ImageMetricsVisualizer() = default;

auto ImageMetricsVisualizer::check_type(const gc::Type* type) -> TypeCheckResult
{
    static auto expected_type = gc::type_of<sieve::ImageMetrics>();

    if (type == expected_type)
        return { .ok = true };

    return {
        .ok = false,
        .expected_type_description = common::format(expected_type)
    };
}

void ImageMetricsVisualizer::set_value(const gc::Value& v)
{
    storage_->view->add_image_metrics(v.as<const sieve::ImageMetrics&>());
}
