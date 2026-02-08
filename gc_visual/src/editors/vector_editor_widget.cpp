/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024-2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "gc_visual/editors/vector_editor_widget.hpp"

#include "plot_visual/color.hpp"
#include "plot_visual/qstr.hpp"

#include <QColorDialog>
#include <QEvent>
#include <QKeyEvent>
#include <QTableView>
#include <QVBoxLayout>

#include <set>

VectorEditorWidget::VectorEditorWidget(const YAML::Node&, QWidget* parent) :
    ParameterEditorWidget{ parent }
{}


auto VectorEditorWidget::maybe_construct(const gc::Value& v) -> void

{
    if (model_)
        return;

    model_ = std::make_unique<VectorItemModel>(v);
    auto* layout = new QVBoxLayout{};
    setLayout(layout);

    view_ = new QTableView{};
    layout->addWidget(view_);

    view_->setModel(model_.get());

    connect(
        model_.get(), &VectorItemModel::dataChanged,
        this,
        [this](const QModelIndex& topLeft,
            const QModelIndex& bottomRight,
            const QList<int>&)
        {
            auto r0 = topLeft.row();
            auto r1 = bottomRight.row();
            auto c0 = topLeft.column();
            auto c1 = bottomRight.column();

            for (auto r=r0; r<=r1; ++r)
                for (auto c=c0; c<=c1; ++c)
                {
                    auto path = model_->path(topLeft.sibling(r, c));
                    auto value = model_->value().get(path);
                    emit value_changed(value, path);
                }
        });

    auto on_resize =
        [this](const QModelIndex& /* parent */, int /* first */, int /* last */)
        { emit value_changed(model_->value(), {}); };

    connect(model_.get(), &VectorItemModel::rowsInserted, this, on_resize);
    connect(model_.get(), &VectorItemModel::rowsRemoved, this, on_resize);
    connect(model_.get(), &VectorItemModel::rowsMoved, this, on_resize);

    auto maybe_edit_color =
        [this](const QModelIndex& index)
    {
        if (!index.isValid())
            return;
        if (static_cast<size_t>(index.row()) == model_->value().size())
            return;
        auto path = model_->path(index);
        auto v = model_->value().get(path);
        if (v.type() != gc::type_of<gc_types::Color>())
            return;

        auto color = v.as<gc_types::Color>();

        auto new_qcolor = QColorDialog::getColor(
            plot::qcolor(color),
            this,
            tr("&Pick color"),
            QColorDialog::ShowAlphaChannel | QColorDialog::DontUseNativeDialog);
        if (!new_qcolor.isValid())
            return;
        v = plot::color(new_qcolor);
        model_->setData(index, plot::format_qstr(v));
    };

    connect(view_, &QTableView::activated, this, maybe_edit_color);
    connect(view_, &QTableView::clicked, this, maybe_edit_color);
    view_->installEventFilter(this);
}

auto VectorEditorWidget::eventFilter(QObject *obj, QEvent *event)
    -> bool
{
    if (obj != view_)
        return false;

    auto index = view_->currentIndex();
    if (!index.isValid())
        return false;

    if (event->type() == QEvent::KeyPress)
    {
        auto key_event = static_cast<QKeyEvent*>(event);
        if (key_event->modifiers() == Qt::NoModifier &&
            key_event->key() == Qt::Key_Insert)
        {
            model_->insertRow(index.row(), index.parent());
            return true;
        }

        if (key_event->modifiers() == Qt::NoModifier &&
            key_event->key() == Qt::Key_Delete)
        {
            auto sel = view_->selectionModel()->selection();
            auto rows = std::set<int>{};
            for (const auto& index : sel.indexes())
                rows.insert(index.row());
            while (!rows.empty())
            {
                auto it = rows.rbegin();
                auto last = *it;
                rows.erase(last);
                model_->removeRow(last);
            }
            return true;
        }
    }
    return false;
}

auto VectorEditorWidget::value() const -> gc::Value
{
    if (!model_)
        return {};

    return model_->value();
}

auto VectorEditorWidget::check_type(const gc::Type* type) -> TypeCheckResult
{
    if (type->aggregate_type() == gc::AggregateType::Vector)
        return { .ok = true };

    return {
        .ok = false,
        .expected_type_description = "a vector type"
    };
}

auto VectorEditorWidget::set_value(const gc::Value& v) -> void
{
    maybe_construct(v);
    model_->setValue(v);
}
