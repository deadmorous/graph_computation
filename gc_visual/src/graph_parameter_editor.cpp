#include "gc_visual/graph_parameter_editor.hpp"

#include "common/throw.hpp"

#include "yaml-cpp/yaml.h"

#include <QSpinBox>
#include <QDoubleSpinBox>


namespace {

template <typename T>
class SpinEditor final
{
public:
    static constexpr auto fp =
        std::is_floating_point_v<T>;

    using EditorDataType =
        std::conditional_t<fp, double, int>;
    using EditorWidgetType =
        std::conditional_t<fp, QDoubleSpinBox, QSpinBox>;

    SpinEditor(T range_min,
               T range_max,
               T value,
               gc::ParameterSpec param_spec,
               GraphBroker* broker,
               QWidget* parent) :
        widget_{ new EditorWidgetType{ parent } },
        param_spec_{ std::move(param_spec) }
    {
        widget_->setMinimum(range_min);
        widget_->setMaximum(range_max);
        widget_->setValue(value);

        QObject::connect(
            widget_, &EditorWidgetType::valueChanged,
            [this, broker](EditorDataType v)
            {
                auto gc_val = gc::Value(common::Type<T>, v);
                broker->set_parameter(param_spec_, gc_val);
            });
    }

private:
    EditorWidgetType* widget_;
    gc::ParameterSpec param_spec_;
};

} // anonymous namespace


GraphParameterEditor::GraphParameterEditor(const std::string& type,
                                           GraphBroker* broker,
                                           const YAML::Node& item_node,
                                           QWidget* parent) :
    QWidget{ parent }
{
    assert(type == "spin");

    // Resolve parameter binding
    auto bind_node = item_node["bind"];
    auto node_name = bind_node["node"].as<std::string>();

    node_ = broker->node(node_name);
    index_ = bind_node["index"].as<uint32_t>();
    path_ = gc::ValuePath::from_string(bind_node["path"].as<std::string>());

    auto param_spec =
        gc::ParameterSpec{ node_, index_, path_ };
    auto value = broker->get_parameter(param_spec);

    auto format_binding = [&]
    { return common::format(node_name, '[', index_, ']', path_); };

    if (value.type()->aggregate_type() != gc::AggregateType::Scalar)
        common::throw_(
            "Invalid binding: 'spin' can only bind to a scalar type, whereas"
            " the parameter ", format_binding(), " is of type ", value.type());

    auto t = gc::ScalarT{ value.type() };

    res_ = t.visit_numeric(
        [&]<typename T>(common::Type_Tag<T>)
            -> std::shared_ptr<void>
        {
            auto range_min = item_node["range"][0].as<T>();
            auto range_max = item_node["range"][1].as<T>();
            using Editor = SpinEditor<T>;
            return std::make_shared<Editor>(range_min,
                                            range_max,
                                            value.as<T>(),
                                            std::move(param_spec),
                                            broker,
                                            this);
        });
}
