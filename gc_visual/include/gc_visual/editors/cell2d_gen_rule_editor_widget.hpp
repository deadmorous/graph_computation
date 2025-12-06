/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "gc_visual/parameter_editor_widget.hpp"

#include <QWidget>

class Cell2dGenRuleEditorWidget final :
    public ParameterEditorWidget
{
    Q_OBJECT

public:
    explicit Cell2dGenRuleEditorWidget(const YAML::Node&,
                                    QWidget* parent = nullptr);

    ~Cell2dGenRuleEditorWidget();

    auto value() const -> gc::Value override;

    static auto check_type(const gc::Type*) -> TypeCheckResult;

public slots:
    void set_value(const gc::Value& v) override;

public:
    struct Storage;

private:
    std::unique_ptr<Storage> storage_;
};
