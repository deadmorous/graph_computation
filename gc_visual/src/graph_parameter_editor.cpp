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
#include "gc_visual/cell2d_rule_editor_widget.hpp"

#include "gc_app/types/color.hpp"

#include "common/throw.hpp"

#include <yaml-cpp/yaml.h>

#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>


using namespace std::literals;

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

class FileEditor final
{
public:

    FileEditor(const std::string& filter,
               const std::string& value,
               gc::ParameterSpec param_spec,
               GraphBroker* broker,
               QWidget* parent) :
        widget_{ new QWidget{ parent } }
    {
        auto* layout = new QHBoxLayout{widget_};
        auto text_input = new QLineEdit{};
        layout->addWidget(text_input);
        auto* button = new QPushButton("&...");
        layout->addWidget(button);

        text_input->setText(QString::fromUtf8(value));

        QObject::connect(
            text_input,
            &QLineEdit::textChanged,
            broker,
            [=](const QString& v)
            {
                auto gc_val = gc::Value(v.toUtf8().toStdString());
                broker->set_parameter(param_spec, gc_val);
            });

        auto open_file = [this, text_input, filter]{
            auto f_info = QFileInfo{text_input->text()};
            auto path = QString{};
            if (f_info.exists())
                path = f_info.filePath();
            else {
                auto parent_dir = f_info.dir();
                if (parent_dir.exists())
                    path = parent_dir.path();
            }

            auto file_name = QFileDialog::getOpenFileName(
                widget_, "Open File",
                path,
                QString::fromUtf8(filter));
            if (file_name.isEmpty())
                return;
            text_input->setText(file_name);
        };

        QObject::connect(button, &QPushButton::clicked, open_file);
    }

    auto widget()
        -> QWidget*
    { return widget_; }

private:
    QWidget* widget_;
};

class Cell2dRuleEditor final
{
public:
    using EditorWidgetType = Cell2dRuleEditorWidget;

    Cell2dRuleEditor(gc::Value value,
                     gc::ParameterSpec param_spec,
                     GraphBroker* broker,
                     QWidget* parent) :
        widget_{ new EditorWidgetType{ std::move(value), parent } }
    {
        QObject::connect(
            widget_,
            &EditorWidgetType::valueChanged,
            broker,
            [=](const gc::Value& v)
            {
                broker->set_parameter(param_spec, v);
            });

        // TODO: Generalize this code for any editor
        if (holds_alternative<gc::NodeOutputSpec>(param_spec.io))
        {
            auto output = get<gc::NodeOutputSpec>(param_spec.io).output;
            QObject::connect(
                broker, &GraphBroker::output_updated,
                [output, broker, param_spec, this](gc::EdgeOutputEnd updated)
                {
                    if (output != updated)
                        return;
                    auto updated_value = broker->get_parameter(param_spec);
                    widget_->setValue(updated_value);
                });
        }
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

auto make_file(const ParamBinding& binding,
               GraphBroker* broker,
               const YAML::Node& item_node)
    -> std::shared_ptr<QWidget>
{
    auto filter = "All files (*)"s;
    if (auto filter_node = item_node["filter"]; filter_node.IsDefined())
        filter = filter_node.as<std::string>();

    auto value = broker->get_parameter(binding.param_spec);

    if (value.type()->aggregate_type() != gc::AggregateType::String)
        common::throw_(
            "Invalid binding: 'file' can only bind to a string type",
            ", whereas" " the parameter ", common::format(binding),
            " is of type ", value.type());

    auto editor
        = std::make_shared<FileEditor>(filter,
                                       value.as<std::string>(),
                                       binding.param_spec,
                                       broker,
                                       nullptr);

    return wrap_edtor(std::move(editor));
}


auto make_cell2d_rules(const ParamBinding& binding,
                       GraphBroker* broker,
                       const YAML::Node& item_node)
    -> std::shared_ptr<QWidget>
{
    auto value = broker->get_parameter(binding.param_spec);

    auto editor
        = std::make_shared<Cell2dRuleEditor>(value,
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
        { "file"sv, make_file },
        { "cell2d_rules"sv, make_cell2d_rules },
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
