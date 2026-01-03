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

#include "gc_visual/editors/parameter_editor_widget.hpp"


class QComboBox;

class ListEditorWidget final :
    public ParameterEditorWidget
{
    Q_OBJECT

public:
    explicit ListEditorWidget(const YAML::Node&, QWidget* parent = nullptr);

    auto value() const -> gc::Value override;

    static auto check_type(const gc::Type*) -> TypeCheckResult;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;
    bool hasHeightForWidth() const override;
    int heightForWidth(int w) const override;

public slots:
    void set_value(const gc::Value& value) override;

private:
    const gc::Type* type_{};
    QComboBox* combo_;
    int in_set_value_{};
};
