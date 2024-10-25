#pragma once

#include "gc_visual/flatten_type.hpp"

#include "gc/value.hpp"

#include <QAbstractListModel>

class VectorItemModel final :
    public QAbstractListModel
{
    Q_OBJECT
public:
    VectorItemModel(gc::Value v, QObject* parent = nullptr);

    auto path(const QModelIndex& index) const
        -> gc::ValuePath;

    auto value() const noexcept
        -> const gc::Value&;

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
    auto setValue(gc::Value v) -> void;

private:
    auto on_set_value(gc::Value v)
        -> void;

    gc::Value v_;
    gc_visual::TypeComponentVec element_fields_;
};
