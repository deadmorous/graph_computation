/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024-2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "gc_visual/editors/parameter_editor_widget.hpp"

class QLineEdit;

class FilePickerWidget final :
    public ParameterEditorWidget
{
    Q_OBJECT

public:
    explicit FilePickerWidget(const YAML::Node& config,
                              QWidget* parent = nullptr);

    auto value() const -> gc::Value override;

    static auto check_type(const gc::Type*) -> TypeCheckResult;

public slots:
    void set_value(const gc::Value& value) override;

private slots:
    void open_file();

private:
    std::string filter_;
    QLineEdit* text_input_{};
};
