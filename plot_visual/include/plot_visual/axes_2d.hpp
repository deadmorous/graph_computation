/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025-2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#pragma once

#include "plot_visual/concepts.hpp"
#include "plot_visual/layout.hpp"

#include "common/defer.hpp"

#include <QFontMetrics>
#include <QPainter>
#include <QRect>
#include <QString>

namespace plot {

template <AxisType XAxis_, AxisType YAxis_>
struct Axes2d final
{
    using XAxis = XAxis_;
    using YAxis = YAxis_;

    XAxis x;
    YAxis y;
    QString title;
};

template <Axes2dType Axes2d>
struct AxesPainter
{
public:
    AxesPainter(const Axes2d& axes, const QRect& rect):
        axes_{axes},
        layout_{rect}
    {
        x_primary_ticks_ = axes_.x.primary_ticks();
        x_secondary_ticks_ = axes_.x.secondary_ticks();
        y_primary_ticks_ = axes_.y.primary_ticks();
        y_secondary_ticks_ = axes_.y.secondary_ticks();

        auto fm = QFontMetrics{QFont{}};
        auto title_size = fm.size(0, axes_.title);

        auto tick_label_size = fm.size(0, "1.234e-10"); // TODO better

        // For tick labels to fit
        layout_.eat(
            layout::Side::Right, tick_label_size.width()/2, layout::central);

        if (!axes_.title.isEmpty())
        {
            title_id_ = layout_.eat(
                layout::Side::Top, title_size.height(), layout::central);
        }

        if (!axes_.x.label.isEmpty())
        {
            auto size = fm.size(0, axes_.x.label);
            x_label_id_ = layout_.eat(
                layout::Side::Bottom, size.height(), layout::central);
        }
        x_tick_label_id_ = layout_.eat(
            layout::Side::Bottom, tick_label_size.height(), layout::central);
        if (axes_.x.show_primary_tick_marks ||
            axes_.x.show_secondary_tick_marks)
        {
            x_tick_mark_id_ = layout_.eat(
                layout::Side::Bottom, primary_tick_mark_size, layout::central);
        }

        if (!axes_.y.label.isEmpty())
        {
            auto size = fm.size(0, axes_.y.label);
            y_label_id_ = layout_.eat(
                layout::Side::Left, size.height(), layout::central);
        }
        y_tick_label_id_ = layout_.eat(
            layout::Side::Left, tick_label_size.width(), layout::central);
        if (axes_.y.show_primary_tick_marks ||
            axes_.y.show_secondary_tick_marks)
        {
            y_tick_mark_id_ = layout_.eat(
                layout::Side::Left, primary_tick_mark_size, layout::central);
        }

        auto central_rect = layout_.rect(layout::central);
        using x_mapped = typename Axes2d::XAxis::CoordinateMapping::To;
        using y_mapped = typename Axes2d::YAxis::CoordinateMapping::To;
        axes_.x.mapping.to = { static_cast<x_mapped>(central_rect.left()),
                               static_cast<x_mapped>(central_rect.right()) };
        axes_.y.mapping.to = { static_cast<y_mapped>(central_rect.bottom()),
                               static_cast<y_mapped>(central_rect.top()) };
    }

    auto axes() const -> const Axes2d&
    {
        return axes_;
    }

    auto layout() noexcept -> layout::Layout&
    {
        return layout_;
    };

    auto layout() const noexcept -> const layout::Layout&
    {
        return layout_;
    };

    auto draw(QPainter& painter)
    {
        painter.save();
        auto restore_painter = common::Defer{ [&] { painter.restore(); } };

        auto h_line_painter = [&](int x0, int x1)
        {
            return [&, x0, x1](auto v)
            {
                auto y = axes_.y.mapping(v);
                painter.drawLine(x0, y, x1, y);
            };
        };

        auto v_line_painter = [&](int y0, int y1)
        {
            return [&, y0, y1](auto v)
            {
                auto x = axes_.x.mapping(v);
                painter.drawLine(x, y0, x, y1);
            };
        };

        auto for_each_tick = [](auto ticks, auto action)
        {
            for (auto tick : ticks)
                action(tick);
        };

        painter.setRenderHint(QPainter::Antialiasing);

        const auto& rc = layout_.rect(layout::central);
        auto x0 = rc.left();
        auto y0 = rc.bottom();
        auto h_grid_line_painter = h_line_painter(x0, rc.right());
        auto v_grid_line_painter = v_line_painter(y0, rc.top());

        constexpr auto secondary_grid_color = QColor(0xcc, 0xcc, 0xcc);
        painter.setPen(QPen(secondary_grid_color, 1, Qt::DotLine));
        if (axes_.x.show_secondary_grid)
            for_each_tick(x_secondary_ticks_, v_grid_line_painter);
        if (axes_.y.show_secondary_grid)
            for_each_tick(y_secondary_ticks_, h_grid_line_painter);

        painter.setPen(QPen(Qt::darkGray, 1, Qt::DotLine));
        if (axes_.x.show_primary_grid)
            for_each_tick(x_primary_ticks_, v_grid_line_painter);
        if (axes_.y.show_primary_grid)
            for_each_tick(y_primary_ticks_, h_grid_line_painter);

        auto fm = painter.fontMetrics();

        painter.setPen(Qt::black);

        if (axes_.x.show_secondary_tick_marks)
            for_each_tick(x_secondary_ticks_,
                          v_line_painter(y0, y0 + secondary_tick_mark_size));

        if (axes_.y.show_secondary_tick_marks)
            for_each_tick(y_secondary_ticks_,
                          h_line_painter(x0, x0 - secondary_tick_mark_size));

        if (axes_.x.show_primary_tick_marks)
            for_each_tick(x_primary_ticks_,
                          v_line_painter(y0, y0 + primary_tick_mark_size));

        if (axes_.y.show_primary_tick_marks)
            for_each_tick(y_primary_ticks_,
                          h_line_painter(x0, x0 - primary_tick_mark_size));

        auto x_label_area_center_y =
            layout_.rect(x_tick_label_id_).center().y();
        auto x_label_right_prev = std::numeric_limits<int>::min();
        for (auto tick : x_primary_ticks_)
        {
            auto x = axes_.x.mapping(tick);
            auto text = QString::number(tick);
            auto text_size = fm.size(0, text);

            auto r = QRect{
                static_cast<int>(x - text_size.width() / 2),
                x_label_area_center_y - text_size.height() / 2,
                text_size.width(),
                text_size.height()
            };
            if (r.left() < x_label_right_prev)
                continue;
            x_label_right_prev = r.right();
            painter.drawText(r, Qt::AlignHCenter | Qt::AlignVCenter, text);
        }

        auto y_label_area_center_x =
            layout_.rect(y_tick_label_id_).center().x();
        auto y_label_top_prev = std::numeric_limits<int>::max();
        for (auto tick : y_primary_ticks_)
        {
            auto y = axes_.y.mapping(tick);
            auto text = QString::number(tick);
            auto text_size = fm.size(0, text);

            auto r = QRect{
                y_label_area_center_x - text_size.width() / 2,
                static_cast<int>(y - text_size.height() / 2),
                text_size.width(),
                text_size.height()
            };
            if (r.bottom() > y_label_top_prev)
                continue;
            y_label_top_prev = r.top();
            painter.drawText(r, Qt::AlignHCenter | Qt::AlignVCenter, text);
        }

        if (!axes_.x.label.isEmpty())
            painter.drawText(layout_.rect(x_label_id_),
                             Qt::AlignHCenter | Qt::AlignVCenter,
                             axes_.x.label);

        if (!axes_.y.label.isEmpty())
        {
            auto restore_transform = common::Defer{
                [&, t = painter.transform()]{ painter.setTransform(t); } };

            auto t = painter.transform();
            auto rect = layout_.rect(y_label_id_);
            auto center = rect.center();
            t.translate(center.x(), center.y());
            t.rotate(-90);
            t.translate(-center.x(), -center.y());
            painter.setTransform(t);
            rect = t.mapRect(rect);
            painter.drawText(rect,
                             Qt::AlignHCenter | Qt::AlignVCenter,
                             axes_.y.label);
        }

        if (!axes_.title.isEmpty())
            painter.drawText(layout_.rect(title_id_),
                             Qt::AlignHCenter | Qt::AlignVCenter,
                             axes_.title);
    }

private:
    using X = typename Axes2d::XAxis::CoordinateMapping::From;
    using Y = typename Axes2d::YAxis::CoordinateMapping::From;

    static constexpr int primary_tick_mark_size = 5;
    static constexpr int secondary_tick_mark_size = 3;

    Axes2d axes_;
    layout::Layout layout_;

    std::span<const X> x_primary_ticks_;
    std::span<const X> x_secondary_ticks_;
    std::span<const Y> y_primary_ticks_;
    std::span<const Y> y_secondary_ticks_;

    layout::ItemIndex title_id_;
    layout::ItemIndex x_label_id_;
    layout::ItemIndex x_tick_label_id_;
    layout::ItemIndex x_tick_mark_id_;
    layout::ItemIndex y_label_id_;
    layout::ItemIndex y_tick_label_id_;
    layout::ItemIndex y_tick_mark_id_;
};

} // namespace plot
