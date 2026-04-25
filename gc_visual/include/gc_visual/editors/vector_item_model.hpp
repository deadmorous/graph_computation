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

#include "gc_visual/flatten_type.hpp"

#include "mpk/mix/value/value.hpp"

#include <QAbstractListModel>

class VectorItemModel final :
    public QAbstractListModel
{
    Q_OBJECT
public:
    VectorItemModel(mpk::mix::value::Value v, QObject* parent = nullptr);

    auto path(const QModelIndex& index) const
        -> mpk::mix::value::ValuePath;

    auto value() const noexcept
        -> const mpk::mix::value::Value&;

    auto rowCount(const QModelIndex &parent = QModelIndex()) const
        -> int override;

    auto columnCount(const QModelIndex &parent = QModelIndex()) const
        -> int override;

    auto flags(const QModelIndex &index) const
        -> Qt::ItemFlags override;

    auto data(const QModelIndex &index, int role = Qt::DisplayRole) const
        -> QVariant override;

    auto setData(const QModelIndex &index,
                 const QVariant &value,
                 int role = Qt::EditRole)
        -> bool override;

    auto headerData(int section,
                    Qt::Orientation orientation,
                    int role = Qt::DisplayRole) const
        -> QVariant override;

    auto insertRows(int row,
                    int count,
                    const QModelIndex &parent = QModelIndex())
        -> bool override;

    auto removeRows(int row,
                    int count,
                    const QModelIndex &parent = QModelIndex())
        -> bool override;

public slots:
    auto setValue(mpk::mix::value::Value v) -> void;

private:
    auto on_set_value(mpk::mix::value::Value v)
        -> void;

    mpk::mix::value::Value v_;
    gc_visual::TypeComponentVec element_fields_;
};
