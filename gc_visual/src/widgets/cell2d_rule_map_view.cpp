/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "gc_visual/widgets/cell2d_rule_map_view.hpp"

#include "common/format.hpp"
#include "common/overloads.hpp"

#include <QBoxLayout>
#include <QCheckBox>
#include <QClipboard>
#include <QLabel>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QSpinBox>

#include <algorithm>
#include <sstream>


auto Cell2dRuleMapView::Layout::size() const noexcept -> QSize
{
    return {
        lh_item_size.width() + th_item_size.width() * column_count,
        th_item_size.height() + lh_item_size.height() * row_count
    };
}

auto Cell2dRuleMapView::Layout::lh_item_rect(int row) const noexcept -> QRect
{
    auto pos = QPoint{0, th_item_size.height() + row * lh_item_size.height()};
    return {pos, lh_item_size};
}

auto Cell2dRuleMapView::Layout::th_item_rect(int col) const noexcept -> QRect
{
    auto pos = QPoint{lh_item_size.width() + col * th_item_size.width(), 0};
    return {pos, th_item_size};
}

auto Cell2dRuleMapView::Layout::item_rect(int row, int col) const noexcept
    -> QRect
{
    auto pos = QPoint{lh_item_size.width() + col * th_item_size.width(),
                      th_item_size.height() + row * lh_item_size.height()};
    auto size = QSize{th_item_size.width(), lh_item_size.height()};
    return {pos, size};
}

auto Cell2dRuleMapView::Layout::item_rect(const RowCol& row_col) const noexcept
    -> QRect
{
    return item_rect(row_col.row, row_col.col);
}

auto Cell2dRuleMapView::Layout::item(const QPoint& pos) const noexcept
    -> Item
{
    if (menu_btn_rect.contains(pos))
        return MenuBtn{};

    if (lh_item_size.width() == 0)
        return NoItem{};
    auto col = (pos.x() - lh_item_size.width()) / th_item_size.width();
    if (col < 0 || col >= column_count)
        return NoItem{};
    if (th_item_size.height() == 0)
        return NoItem{};
    auto row = (pos.y() - th_item_size.height()) / lh_item_size.height();
    if (row < 0 || row >= row_count)
        return NoItem{};
    return RowCol{row, col};
}

// ---

Cell2dRuleMapView::Cell2dRuleMapView(uint8_t max_neighborhood_size,
                                     QWidget* parent) :
    QWidget{parent},
    max_neighborhood_size_{max_neighborhood_size}
{
    font_.setPointSize(font_.pointSize() * 3 / 4);

    menu_ = new QMenu{this};
    connect(menu_->addAction("&Copy"), &QAction::triggered,
            [&]{
                auto m = std::vector<int>{};
                m.assign(map_.begin(), map_.end());
                m.resize(map_size(), 0);
                auto text = common::format_seq(m, " ");
                qApp->clipboard()->setText(QString::fromUtf8(text));
            });

    paste_action_ = menu_->addAction("&Paste");
    connect(paste_action_, &QAction::triggered,
            [&]{
                auto text = qApp->clipboard()->text().toStdString();
                if (text.size() == 0)
                    return;
                auto m = std::vector<int8_t>{};
                auto s = std::istringstream{text};
                while (true)
                {
                    int n;
                    s >> n;
                    if (s.fail())
                        break;
                    m.push_back(n);
                }
                if (m.empty())
                    return;
                m.resize(map_size(), 0);
                if (m == map_)
                    return;
                set_map(m);
                emit map_changed(map_);
            });

    menu_->addSeparator();

    connect(menu_->addAction("Set &no-op rules"), &QAction::triggered,
            [&]{
                auto m = std::vector<int8_t>(map_.size(), -128);
                if (m == map_)
                    return;
                set_map(m);
                emit map_changed(map_);
            });
}

auto Cell2dRuleMapView::map() const noexcept
    -> std::vector<int8_t>
{
    return map_;
}

auto Cell2dRuleMapView::set_map(const std::vector<int8_t>& map) -> void
{
    map_ = map;
    calc_layout();
    update();
}

auto Cell2dRuleMapView::set_state_count(uint8_t state_count) -> void
{
    state_count_ = state_count;
    map_.resize(map_size(), 0);
    calc_layout();
    update();
}

auto Cell2dRuleMapView::set_min_state(int8_t min_state) -> void
{
    auto delta = min_state - min_state_;
    for (auto& v : map_)
        if (v != -128)
            v += delta;
    min_state_ = min_state;
    calc_layout();
    update();
}

auto Cell2dRuleMapView::paintEvent(QPaintEvent*)
    -> void
{
    auto painter = QPainter{this};
    painter.setFont(font_);
    painter.setRenderHint(QPainter::Antialiasing);

    for (int row=0; row<layout_.row_count; ++row)
    {
        auto rc = layout_.lh_item_rect(row);
        auto state = int{min_state_} + row;
        painter.drawText(
            rc, Qt::AlignHCenter | Qt::AlignVCenter, QString::number(state));
    }

    for (int min_sum = int{min_state_} * max_neighborhood_size_,
         col=0; col<layout_.column_count; ++col)
    {
        auto rc = layout_.th_item_rect(col);
        auto sum = min_sum + col;
        if (abs(sum) % layout_.th_skip_factor != 0)
            continue;
        auto dw = layout_.th_text_width - rc.width();
        if (dw > 0)
        {
            auto dw1 = dw / 2;
            auto dw2 = dw - dw1;
            rc.adjust(-dw1, 0, dw2, 0);
        }
        painter.drawText(
            rc, Qt::AlignHCenter | Qt::AlignVCenter, QString::number(sum));
    }

    auto small_rect = [](QRect rc)
    {
        auto s = std::min(rc.width(), rc.height()) - 4;
        return QRect{rc.center() - QPoint{s/2, s/2}, QSize{s, s}};
    };

    auto fillEllipse = [&](QRect rc, QColor color)
    {
        painter.setPen(Qt::NoPen);
        painter.setBrush(color);
        painter.drawEllipse(rc);
    };

    auto fill_column = [&](int col, QColor color)
    {
        for (int row=0; row<layout_.row_count; ++row)
        {
            auto rc = layout_.item_rect(row, col);
            fillEllipse(small_rect(rc), color);
        }
    };

    auto min_state = int{min_state_};
    for (int col=0; col<layout_.column_count; ++col)
    {
        if (col < map_.size())
        {
            int row = map_[col];
            if (row == -128)
            {
                fill_column(col, Qt::darkGray);
                continue;
            }

            row -= min_state;
            if (row >= 0 && row < layout_.row_count)
            {
                auto rc = layout_.item_rect(row, col);
                fillEllipse(small_rect(rc), Qt::black);
                continue;
            }
        }
        fill_column(col, Qt::red);
    }

    auto draw_hline = [&](int y) { painter.drawLine(0, y, width(), y); };
    auto draw_vline = [&](int x) { painter.drawLine(x, 0, x, height()); };

    painter.setPen(Qt::darkGray);
    draw_hline(layout_.th_item_size.height());
    draw_vline(layout_.lh_item_size.width());
    {
        painter.setBrush(Qt::gray);
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(layout_.menu_btn_rect, 2., 2.);
    }
}

auto Cell2dRuleMapView::mouseReleaseEvent(QMouseEvent *event)
    -> void
{
    if (event->button() != Qt::LeftButton)
        return;
    auto ctrl = (event->modifiers() & Qt::ControlModifier) != 0;
    visit(common::Overloads{
        [](NoItem) {},
        [&](MenuBtn) {
            paste_action_->setDisabled(qApp->clipboard()->text().isEmpty());
            menu_->exec(mapToGlobal(event->pos()));
        },
        [&](const RowCol& item) {
            if (item.col >= map_.size())
                return;
            auto new_value = ctrl ? -128 : int8_t(item.row) + min_state_;
            auto& current_value = map_[item.col];
            if (new_value == current_value)
                return;
            current_value = new_value;
            emit map_changed(map_);
            update();
        }
    }, layout_.item(event->pos()));
}

auto Cell2dRuleMapView::map_size() const noexcept -> size_t
{
    return max_neighborhood_size_ * (size_t(state_count_) - 1u) + 1;
}

auto Cell2dRuleMapView::calc_layout() -> void
{
    auto fm = QFontMetrics{font_};
    auto min_state = int{min_state_};
    auto max_state = int{min_state_} + state_count_ - 1;
    auto min_sum = min_state * max_neighborhood_size_;
    auto max_sum = max_state * max_neighborhood_size_;

    auto header_item_size = [&](int min, int max) -> QSize
    {
        constexpr int h_margin = 3;
        constexpr int v_margin = 3;

        auto min_size =
            fm.size(Qt::TextSingleLine, QString::number(min));
        auto max_size =
            fm.size(Qt::TextSingleLine, QString::number(max));
        return {
            std::max(min_size.width(), max_size.width()) + 2*h_margin,
            std::max(min_size.height(), max_size.height()) + 2*h_margin
        };
    };

    layout_.lh_item_size = header_item_size(min_state, max_state);
    layout_.th_item_size = header_item_size(0, 1);
    layout_.column_count = max_sum - min_sum + 1;
    layout_.row_count = state_count_;

    auto th_width = layout_.th_item_size.width();
    auto th_text_width = header_item_size(min_sum, max_sum).width();
    auto factor_ok = [&](int factor)
    { return factor * th_width >= th_text_width; };
    auto skip_factor = 1;
    while (!factor_ok(skip_factor))
    {
        if (factor_ok(skip_factor * 2))
        {
            skip_factor *= 2;
            break;
        }
        if (factor_ok(skip_factor * 5))
        {
            skip_factor *= 5;
            break;
        }
        skip_factor *= 10;
    }
    layout_.th_skip_factor = skip_factor;
    layout_.th_text_width = th_text_width;

    {
        constexpr int hm = 3;
        constexpr int vm = 5;
        auto rc = QRect(
            0, 0, layout_.lh_item_size.width(), layout_.th_item_size.height());
        rc.adjust(hm, vm, -hm, -vm);
        layout_.menu_btn_rect = rc;
    }

    setMinimumSize(layout_.size());
}
