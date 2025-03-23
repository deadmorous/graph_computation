/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "gc_visual/vector_item_model.hpp"

#include <QWidget>

class QTableView;

class VectorEditorWidget final :
    public QWidget
{
    Q_OBJECT
public:
    VectorEditorWidget(gc::Value v, QWidget* parent = nullptr);

    auto value() const noexcept
        -> const gc::Value&;

signals:
    auto valueChanged(const gc::Value& v, gc::ValuePathView path) -> void;

public slots:
    auto setValue(gc::Value v) -> void;

protected:
    auto eventFilter(QObject *obj, QEvent *event) -> bool override;

private:
    VectorItemModel model_;
    QTableView* view_;
};
