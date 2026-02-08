/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024-2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "gc_visual/editors/vector_item_model.hpp"

#include "plot_visual/color.hpp"
#include "plot_visual/qstr.hpp"

#include "gc/parse_simple_value.hpp"

#include <QMessageBox>


VectorItemModel::VectorItemModel(gc::Value v, QObject* parent) :
    QAbstractListModel{ parent }
{ on_set_value(std::move(v)); }

auto VectorItemModel::path(const QModelIndex& index) const
    -> gc::ValuePath
{
    const auto& field = element_fields_.at(index.column());
    auto row = index.row();
    return gc::ValuePathItem{uint32_t(row)} / field.path;
}

auto VectorItemModel::value() const noexcept
    -> const gc::Value&
{ return v_; }

auto VectorItemModel::setValue(gc::Value v) -> void
{ on_set_value(std::move(v)); }

auto VectorItemModel::on_set_value(gc::Value v)
    -> void
{
    const auto* t = v.type();
    if (t->aggregate_type() != gc::AggregateType::Vector)
        common::throw_("VectorItemModel requires a vector type, got ", t);

    auto vt = gc::VectorT{ t }.element_type();
    auto element_fields = gc_visual::flatten_type(vt);

    beginResetModel();
    v_ = std::move(v);
    element_fields_ = std::move(element_fields);
    endResetModel();
}

auto VectorItemModel::rowCount(const QModelIndex &parent) const
    -> int
{
    if (parent.isValid())
        return 0;

    return v_.size() + 1;
}

auto VectorItemModel::columnCount(const QModelIndex &parent) const
    -> int
{
    if (parent.isValid())
        return 0;

    return element_fields_.size();
}

auto VectorItemModel::flags(const QModelIndex &index) const
    -> Qt::ItemFlags
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    auto result = Qt::ItemIsSelectable | Qt::ItemIsEnabled;

    if (static_cast<size_t>(index.row()) == v_.size())
        return result;

    result |= Qt::ItemIsEditable;

    const auto& field = element_fields_.at(index.column());
    if (field.type == gc::type_of<bool>())
        result |= Qt::ItemIsUserCheckable;

    return result;
}

auto VectorItemModel::data(const QModelIndex &index, int role) const
    -> QVariant
{
    if (!index.isValid())
        return {};

    const auto& field = element_fields_.at(index.column());

    auto row = index.row();
    auto path = gc::ValuePathItem{uint32_t(row)} / field.path;

    switch (role)
    {
    case Qt::DisplayRole:
    {
        if (static_cast<size_t>(row) == v_.size())
            return tr("<new>");

        if (field.type == gc::type_of<gc_types::Color>())
            return "#" + QString::number(v_.get(path).as<gc_types::Color>().v, 16);

        return plot::format_qstr(v_.get(path));
    }
    case Qt::EditRole:
    {
        if (static_cast<size_t>(row) == v_.size())
            return {};

        if (field.type == gc::type_of<gc_types::Color>())
            return "#" + QString::number(v_.get(path).as<gc_types::Color>().v, 16);

        return plot::format_qstr(v_.get(path));
    }
    case Qt::BackgroundRole:
    {
        if (static_cast<size_t>(row) == v_.size())
            return {};

        if (field.type == gc::type_of<gc_types::Color>())
            return plot::qcolor(v_.get(path).as<gc_types::Color>());

        return {};
    }
    }

    return {};
}

auto VectorItemModel::setData(const QModelIndex &index,
                              const QVariant &value,
                              int role)
    -> bool
{
    if (!index.isValid())
        return false;

    if (role != Qt::EditRole)
        return false;

    const auto& field = element_fields_.at(index.column());

    auto row = index.row();
    auto path = gc::ValuePathItem{uint32_t(row)} / field.path;

    if (static_cast<size_t>(row) == v_.size())
        return false;

    try {
        auto text = value.toString().toStdString();

        auto item_value = parse_simple_value(text, field.type);
        v_.set(path, item_value);
        emit dataChanged(index, index);
        return true;
    }
    catch(std::exception& e)
    {
        QMessageBox::critical(
            nullptr,
            tr("Set value"),
            plot::format_qstr("Failed to set item value: ", e.what()));
        return false;
    }
}

auto VectorItemModel::headerData(int section,
                                 Qt::Orientation orientation,
                                 int role) const
    -> QVariant
{
    if (role != Qt::DisplayRole)
        return {};

    switch(orientation)
    {
    case Qt::Vertical:
        return section;
    case Qt::Horizontal:
    {
        const auto& path = element_fields_.at(section).path;
        if (path.empty())
            return tr("element");
        else
            return plot::format_qstr(path).mid(1);    // Skip leading slash
    }
    }
    __builtin_unreachable();
}

auto VectorItemModel::insertRows(int row, int count, const QModelIndex &parent)
    -> bool
{
    int old_size = v_.size();

    if (row < 0 || row > old_size || count < 1)
        return false;

    beginInsertRows(parent, row, row+count-1);

    v_.resize(old_size + count);

    for (int i=old_size-1; i>=row; --i)
    {
        auto element = v_.get(gc::ValuePath{size_t(i)});
        v_.set(gc::ValuePath{size_t(i+count)}, element);
    }

    endInsertRows();
    return false;
}

auto VectorItemModel::removeRows(int row, int count, const QModelIndex &parent)
    -> bool
{
    int old_size = v_.size();

    if (row < 0 || count < 1 || row+count > old_size)
        return false;

    beginRemoveRows(parent, row, row+count-1);

    for (int i=row; i+count<old_size; ++i)
    {
        auto element = v_.get(gc::ValuePath{size_t(i+count)});
        v_.set(gc::ValuePath{size_t(i)}, element);
    }

    v_.resize(old_size - count);

    endRemoveRows();
    return false;
}
