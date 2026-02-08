/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024-2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "gc_visual/editors/parameter_editor_widget.hpp"

#include "gc_types/color.hpp"


class ColorEditorWidget final :
    public ParameterEditorWidget
{
    Q_OBJECT

public:
    explicit ColorEditorWidget(const YAML::Node&, QWidget* parent = nullptr);

    auto value() const -> gc::Value override;

    static auto check_type(const gc::Type*) -> TypeCheckResult;

public slots:
    void set_value(const gc::Value& value) override;

protected:
    auto paintEvent(QPaintEvent*)
        -> void override;

    auto mouseReleaseEvent(QMouseEvent *event)
        -> void override;

private:
    gc_types::Color color_;
};
