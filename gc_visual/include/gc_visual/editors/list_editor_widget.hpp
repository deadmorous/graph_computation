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

#include <QComboBox>

class ListEditorWidget final :
    public ParameterEditorWidgetWrapper<QComboBox>
{
    Q_OBJECT

public:
    explicit ListEditorWidget(const YAML::Node&, QWidget* parent = nullptr);

    auto value() const -> mpk::mix::value::Value override;

    static auto check_type(const mpk::mix::value::Type*) -> TypeCheckResult;

public slots:
    void set_value(const mpk::mix::value::Value& value) override;

private:
    const mpk::mix::value::Type* type_{};
    int in_set_value_{};
};
