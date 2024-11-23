#include "gc_visual/graph_parameter_editor.hpp"

#include "gc_visual/color_editor_widget.hpp"
#include "gc_visual/qstr.hpp"
#include "gc_visual/vector_editor_widget.hpp"

#include "gc_app/color.hpp"

#include "gc/computation_node.hpp"

#include "common/throw.hpp"

#include "yaml-cpp/yaml.h"

#include <QDoubleSpinBox>
#include <QLabel>
#include <QSpinBox>
#include <QVBoxLayout>


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
        widget_{ new EditorWidgetType{ parent } }
    {
        widget_->setMinimum(range_min);
        widget_->setMaximum(range_max);
        widget_->setValue(value);

        QObject::connect(
            widget_,
            &EditorWidgetType::valueChanged,
            broker,
            [=](EditorDataType v)
            {
                auto gc_val = gc::Value(common::Type<T>, v);
                broker->set_parameter(param_spec, gc_val);
            });
    }

    auto widget()
        -> EditorWidgetType*
    { return widget_; }

private:
    EditorWidgetType* widget_;
};

class ColorEditor final
{
public:
    using EditorWidgetType = ColorEditorWidget;

    ColorEditor(const gc_app::Color& color,
                gc::ParameterSpec param_spec,
                GraphBroker* broker,
                QWidget* parent) :
        widget_{ new EditorWidgetType{ color, parent } }
    {
        QObject::connect(
            widget_,
            &EditorWidgetType::valueChanged,
            broker,
            [=](gc_app::Color v){ broker->set_parameter(param_spec, v); } );
    }

    auto widget()
        -> EditorWidgetType*
    { return widget_; }

private:
    EditorWidgetType* widget_;
};

class VectorEditor final
{
public:
    using EditorWidgetType = VectorEditorWidget;

    VectorEditor(gc::Value value,
                 gc::ParameterSpec param_spec,
                 GraphBroker* broker,
                 QWidget* parent) :
        widget_{ new EditorWidgetType{ std::move(value), parent } }
    {
        QObject::connect(
            widget_,
            &EditorWidgetType::valueChanged,
            broker,
            [=](const gc::Value& v, gc::ValuePathView path)
            {
                auto subspec = param_spec;
                subspec.path /= path;
                broker->set_parameter(subspec, v);
            });
    }

    auto widget()
        -> EditorWidgetType*
    { return widget_; }

private:
    EditorWidgetType* widget_;
};

// ---

template <typename Editor>
auto wrap_edtor(std::shared_ptr<Editor> editor)
    -> std::shared_ptr<QWidget>
{
    return { editor->widget(),
             [e=std::move(editor)](QWidget*) mutable { e.reset(); } };
}

// ---

struct ParamBinding
{
    gc::ParameterSpec   param_spec;
    std::string         input_name;
};

auto operator<<(std::ostream& s, const ParamBinding& binding)
    -> std::ostream&
{
    return
        s << binding.input_name
          << binding.param_spec.path;
}

auto binding_label(const ParamBinding& binding)
    -> std::string
{
    auto result = binding.input_name;
    if (!binding.param_spec.path.empty())
        result += common::format(binding.param_spec.path);
    return result;
}

auto parse_param_binding(GraphBroker* broker,
                         const YAML::Node& item_node)
    -> ParamBinding
{
    // Resolve parameter binding
    auto input_name = item_node["bind"].as<std::string>();

    auto path = gc::ValuePath{};
    auto path_pos = input_name.find_first_of('/');
    if (path_pos != std::string::npos)
    {
        path = gc::ValuePath::from_string(input_name.substr(path_pos));
        input_name = input_name.substr(0, path_pos);
    }

    auto input = broker->input_index(input_name);

    return { { input, path }, std::move(input_name) };
}

// ---

auto make_spin(const ParamBinding& binding,
               GraphBroker* broker,
               const YAML::Node& item_node)
    -> std::shared_ptr<QWidget>
{
    auto value = broker->get_parameter(binding.param_spec);

    if (value.type()->aggregate_type() != gc::AggregateType::Scalar)
        common::throw_(
            "Invalid binding: 'spin' can only bind to a scalar type, whereas"
            " the parameter ", common::format(binding),
            " is of type ", value.type());

    auto t = gc::ScalarT{ value.type() };

    return t.visit_numeric(
        [&]<typename T>(common::Type_Tag<T>)
        {
            auto range_min = item_node["range"][0].as<T>();
            auto range_max = item_node["range"][1].as<T>();
            using Editor = SpinEditor<T>;
            auto editor
                = std::make_shared<Editor>(range_min,
                                           range_max,
                                           value.as<T>(),
                                           binding.param_spec,
                                           broker,
                                           nullptr);

            return wrap_edtor(std::move(editor));
        });
}

auto make_color(const ParamBinding& binding,
                GraphBroker* broker,
                const YAML::Node& item_node)
    -> std::shared_ptr<QWidget>
{
    auto value = broker->get_parameter(binding.param_spec);

    auto color_type = gc::type_of<gc_app::Color>();
    if (value.type() != color_type)
        common::throw_(
            "Invalid binding: 'color' can only bind to a ", color_type,
            ", whereas" " the parameter ", common::format(binding),
            " is of type ", value.type());

    auto editor
        = std::make_shared<ColorEditor>(value.as<gc_app::Color>(),
                                        binding.param_spec,
                                        broker,
                                        nullptr);

    return wrap_edtor(std::move(editor));
}

auto make_vector(const ParamBinding& binding,
                 GraphBroker* broker,
                 const YAML::Node& item_node)
    -> std::shared_ptr<QWidget>
{
    auto value = broker->get_parameter(binding.param_spec);

    if (value.type()->aggregate_type() != gc::AggregateType::Vector)
        common::throw_(
            "Invalid binding: 'vector' can only bind to a vector type",
            ", whereas" " the parameter ", common::format(binding),
            " is of type ", value.type());

    auto editor
        = std::make_shared<VectorEditor>(value,
                                         binding.param_spec,
                                         broker,
                                         nullptr);

    return wrap_edtor(std::move(editor));
}

} // anonymous namespace


GraphParameterEditor::GraphParameterEditor(const std::string& type,
                                           GraphBroker* broker,
                                           const YAML::Node& item_node,
                                           QWidget* parent) :
    QWidget{ parent }
{
    auto binding =
        parse_param_binding(broker, item_node);

    if(type == "spin")
        res_ = make_spin(binding, broker, item_node);
    else if (type == "color")
        res_ = make_color(binding, broker, item_node);
    else if (type == "vector")
        res_ = make_vector(binding, broker, item_node);
    else
        common::throw_("Unknown graph parameter ediror type '", type, '\'');

    auto layout = new QVBoxLayout{};
    setLayout(layout);
    auto* label =
        new QLabel(qstr(binding_label(binding)));
    label->setBuddy(res_.get());
    layout->addWidget(label);
    layout->addWidget(res_.get());
}
