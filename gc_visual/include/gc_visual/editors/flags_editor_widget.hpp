/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "gc_visual/editors/parameter_editor_widget_wrapper.hpp"

#include "mpk/mix/value/value.hpp"

#include <QToolButton>

class FlagsEditorWidget final :
    public ParameterEditorWidgetWrapper<QToolButton>
{
    Q_OBJECT

public:
    explicit FlagsEditorWidget(const YAML::Node&, QWidget* parent = nullptr);

    auto value() const -> mpk::mix::value::Value override;

    static auto check_type(const mpk::mix::value::Type*) -> TypeCheckResult;

public slots:
    void set_value(const mpk::mix::value::Value& value) override;

private:
    auto update_summary() -> void;
    auto summary() const -> std::string;

    mpk::mix::value::Value value_;
    int in_set_value_{};
};
