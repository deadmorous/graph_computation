/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024-2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "gc_visual/graph_parameter_editor.hpp"

#include "gc_visual/color_editor_widget.hpp"
#include "gc_visual/parse_graph_binding.hpp"
#include "gc_visual/qstr.hpp"
#include "gc_visual/vector_editor_widget.hpp"

#include "gc_app/types/color.hpp"

#include "common/throw.hpp"

#include <yaml-cpp/yaml.h>

#include <QDoubleSpinBox>
#include <QLabel>
#include <QSpinBox>
#include <QVBoxLayout>


using namespace std::string_view_literals;

namespace {

using gc_visual::ParamBinding;

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

using GraphParameterEditorFactoryFunc =
    std::shared_ptr<QWidget>(*)(
        const ParamBinding&, GraphBroker*, const YAML::Node&);

using GraphParameterEditorFactoryMap =
    std::unordered_map<std::string_view, GraphParameterEditorFactoryFunc>;

auto editor_factory_map() -> const GraphParameterEditorFactoryMap&
{
    static auto result = GraphParameterEditorFactoryMap{
        { "spin"sv, make_spin },
        { "color"sv, make_color },
        { "vector"sv, make_vector },
    };

    return result;
}

} // anonymous namespace


GraphParameterEditor::GraphParameterEditor(const std::string& type,
                                           GraphBroker* broker,
                                           const YAML::Node& item_node,
                                           QWidget* parent) :
    QWidget{ parent }
{
    auto binding =
        gc_visual::parse_param_binding(broker->binding_resolver(), item_node);

    res_ = editor_factory_map().at(type)(binding, broker, item_node);

    auto layout = new QVBoxLayout{};
    setLayout(layout);
    auto* label =
        new QLabel(qstr(gc_visual::param_binding_label(binding)));
    label->setBuddy(res_.get());
    layout->addWidget(label);
    layout->addWidget(res_.get());
}

auto GraphParameterEditor::supports_type(const std::string& type) -> bool
{
    return editor_factory_map().contains(type);
}
