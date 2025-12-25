/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024-2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "gc_visual/widgets/graph_parameter_editor.hpp"

#include "gc_visual/editors/cell2d_gen_cmap_editor_widget.hpp"
#include "gc_visual/editors/cell2d_gen_rule_editor_widget.hpp"
#include "gc_visual/editors/cell2d_rule_editor_widget.hpp"
#include "gc_visual/editors/color_editor_widget.hpp"
#include "gc_visual/editors/file_picker_widget.hpp"
#include "gc_visual/editors/spin_editor_widget.hpp"
#include "gc_visual/editors/vector_editor_widget.hpp"
#include "gc_visual/graph_broker.hpp"
#include "gc_visual/parse_graph_binding.hpp"
#include "gc_visual/qstr.hpp"

#include "common/func_ref.hpp"
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

#include <concepts>

using namespace std::literals;

namespace {

using gc_visual::ParamBinding;

template<std::derived_from<ParameterEditorWidget> EditorWidget>
class Editor final
{
public:

    template <typename... WidgetCtorArgs>
    Editor(const std::string& editor_type,
           const ParamBinding& binding,
           GraphBroker* broker,
           QWidget* parent,
           WidgetCtorArgs&&... widget_ctor_args)
    {
        auto value = broker->get_parameter(binding.param_spec);

        if (auto type_check_result = EditorWidget::check_type(value.type());
            !type_check_result.ok)
        {
            common::throw_(
                "Invalid binding: '", editor_type,
                "' can only bind to ",
                type_check_result.expected_type_description,
                ", whereas the parameter ", common::format(binding),
                " is of type ", value.type());
        }

        auto param_spec = binding.param_spec;

        widget_ = new EditorWidget{
                std::forward<WidgetCtorArgs>(widget_ctor_args)...,
                parent };
        QObject::connect(
            widget_,
            &EditorWidget::value_changed,
            broker,
            [=](const gc::Value& v, gc::ValuePathView path)
            {
                auto subspec = param_spec;
                subspec.path /= path;
                broker->set_parameter(subspec, v);
            });

        widget_->set_value(value);

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
                    widget_->set_value(updated_value);
                });
        }
    }

    auto widget()
        -> EditorWidget*
    { return widget_; }

private:
    EditorWidget* widget_{};
};

using EditorFactoryFunc =
    common::FuncRef<std::shared_ptr<QWidget>(
        const std::string&,
        const ParamBinding&,
        GraphBroker*,
        const YAML::Node&)>;

template <std::derived_from<ParameterEditorWidget> EditorWidget>
auto editor_factory() -> EditorFactoryFunc
{
    static constexpr auto impl = [](const std::string& editor_type,
                                    const ParamBinding& binding,
                                    GraphBroker* broker,
                                    const YAML::Node& item_node)
        -> std::shared_ptr<QWidget>
    {
        auto editor = std::make_shared<Editor<EditorWidget>>(
            editor_type,
            binding,
            broker,
            nullptr,
            item_node);
        return { editor->widget(),
                [e=std::move(editor)](QWidget*) mutable { e.reset(); } };
    };
    return &impl;
}

using GraphParameterEditorFactoryMap =
    std::unordered_map<std::string_view, EditorFactoryFunc>;

auto editor_factory_map() -> const GraphParameterEditorFactoryMap&
{
    static auto result = GraphParameterEditorFactoryMap{
        { "cell2d_gen_cmap"sv, editor_factory<Cell2dGenCmapEditorWidget>() },
        { "cell2d_gen_rules"sv, editor_factory<Cell2dGenRuleEditorWidget>() },
        { "cell2d_rules"sv, editor_factory<Cell2dRuleEditorWidget>() },
        { "color"sv, editor_factory<ColorEditorWidget>() },
        { "file"sv, editor_factory<FilePickerWidget>() },
        { "spin"sv, editor_factory<SpinEditorWidget>() },
        { "vector"sv, editor_factory<VectorEditorWidget>() },
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

    res_ = editor_factory_map().at(type)(type, binding, broker, item_node);

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
