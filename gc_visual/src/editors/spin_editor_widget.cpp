/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "gc_visual/editors/spin_editor_widget.hpp"

#include "gc/value.hpp"

#include <yaml-cpp/yaml.h>

#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QSpinBox>

SpinEditorWidget::SpinEditorWidget(const YAML::Node& config,
                                   QWidget* parent) :
    ParameterEditorWidget{ parent },
    config_{ config }
{}

auto SpinEditorWidget::value() const -> gc::Value
{
    if (!value_proxy_)
        return {};

    return value_proxy_->get();
}

auto SpinEditorWidget::check_type(const gc::Type* type) -> TypeCheckResult
{
    if (type->aggregate_type() == gc::AggregateType::Scalar)
        return { .ok = true };

    return {
        .ok = false,
        .expected_type_description = "a scalar type"
    };
}

void SpinEditorWidget::set_value(const gc::Value& value)
{
    maybe_construct(value);
    value_proxy_->set(value);
}

auto SpinEditorWidget::maybe_construct(const gc::Value& value) -> void
{
    if (value_proxy_)
        return;

    auto t = gc::ScalarT{ value.type() };
    t.visit_numeric(
        [&]<typename T>(common::Type_Tag<T>)
        {
            auto range_min = config_["range"][0].as<T>();
            auto range_max = config_["range"][1].as<T>();

            static constexpr auto fp =
                std::is_floating_point_v<T>;

            using EditorDataType =
                std::conditional_t<fp, double, int>;
            using EditorWidgetType =
                std::conditional_t<fp, QDoubleSpinBox, QSpinBox>;

            class Proxy final : public ValueProxy
            {
            public:
                explicit Proxy(EditorWidgetType* widget):
                    widget_{widget}
                {}

                auto set(const gc::Value& v) -> void
                { widget_->setValue(v.convert_to<T>()); }

                auto get() -> gc::Value
                { return widget_->value(); }

                EditorWidgetType* widget_;
            };

            auto* layout = new QHBoxLayout{};
            setLayout(layout);
            auto* widget = new EditorWidgetType{};
            widget->setMinimum(range_min);
            widget->setMaximum(range_max);
            layout->addWidget(widget);

            connect(widget, &EditorWidgetType::valueChanged,
                    this,
                    [this](EditorDataType v) {
                        emit value_changed(static_cast<T>(v));
                    });

            value_proxy_ = std::make_unique<Proxy>(widget);
        });
}
