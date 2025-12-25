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
#include "gc_visual/editors/vector_item_model.hpp"

#include <QWidget>

#include <memory>

class QTableView;

class VectorEditorWidget final :
    public ParameterEditorWidget
{
    Q_OBJECT

public:
    explicit VectorEditorWidget(const YAML::Node&, QWidget* parent = nullptr);

    auto value() const -> gc::Value override;

    static auto check_type(const gc::Type*) -> TypeCheckResult;

public slots:
    void set_value(const gc::Value& v) override;

protected:
    auto eventFilter(QObject *obj, QEvent *event) -> bool override;

private:
    auto maybe_construct(const gc::Value& v) -> void;

    std::unique_ptr<VectorItemModel> model_;
    QTableView* view_;
};
