#include "gc_visual/vector_editor_widget.hpp"

#include "gc_visual/color.hpp"
#include "gc_visual/qstr.hpp"

#include <QColorDialog>
#include <QEvent>
#include <QKeyEvent>
#include <QTableView>
#include <QVBoxLayout>

#include <set>

VectorEditorWidget::VectorEditorWidget(gc::Value v, QWidget* parent) :
    QWidget{ parent },
    model_{ v }
{
    auto* layout = new QVBoxLayout{};
    setLayout(layout);

    view_ = new QTableView{};
    layout->addWidget(view_);

    view_->setModel(&model_);

    connect(
        &model_, &VectorItemModel::dataChanged,
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
                    auto path = model_.path(topLeft.sibling(r, c));
                    auto value = model_.value().get(path);
                    emit valueChanged(value, path);
                }
        });

    auto on_resize = [this](const QModelIndex &parent, int first, int last)
    { emit valueChanged(model_.value(), {}); };

    connect(&model_, &VectorItemModel::rowsInserted, this, on_resize);
    connect(&model_, &VectorItemModel::rowsRemoved, this, on_resize);
    connect(&model_, &VectorItemModel::rowsMoved, this, on_resize);

    auto maybe_edit_color =
        [this](const QModelIndex& index)
    {
        if (!index.isValid())
            return;
        if (index.row() == model_.value().size())
            return;
        auto path = model_.path(index);
        auto v = model_.value().get(path);
        if (v.type() != gc::type_of<gc_app::Color>())
            return;

        auto color = v.as<gc_app::Color>();

        auto new_qcolor = QColorDialog::getColor(
            gc_visual::qcolor(color),
            this,
            tr("&Pick color"),
            QColorDialog::ShowAlphaChannel | QColorDialog::DontUseNativeDialog);
        if (!new_qcolor.isValid())
            return;
        v = gc_visual::color(new_qcolor);
        model_.setData(index, format_qstr(v));
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
            model_.insertRow(index.row(), index.parent());
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
                model_.removeRow(last);
            }
            return true;
        }
    }
    return false;
}

auto VectorEditorWidget::value() const noexcept
    -> const gc::Value&
{ return model_.value(); }

auto VectorEditorWidget::setValue(gc::Value v) -> void
{ model_.setValue(std::move(v)); }
