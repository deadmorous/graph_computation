/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#pragma once

#include <QPainter>
#include <QPixmap>
#include <QSize>

#include <concepts>
#include <functional>


class QPainter;

namespace plot::detail {

class XIncrementalDrawing final
{
public:
    class Updater final
    {
    friend class XIncrementalDrawing;
    public:
        template <std::invocable<QPainter&, double> F>
        auto operator()(double width, F&& update) -> void
        {
            auto x = drawing_.end_;
            if (x + drawing_.tol_ > drawing_.pixmap_.width())
            {
                x = 0;
                drawing_.wrapped_ = true;
            }
            drawing_.end_ = x + width;
            std::invoke(std::forward<F>(update), painter_, x);
        }

    private:
        explicit Updater(XIncrementalDrawing& drawing) :
            drawing_{ drawing },
            painter_{ &drawing_.pixmap_ }
        {}

        XIncrementalDrawing& drawing_;
        QPainter painter_;
    };
    friend class Updater;

    auto resize(const QSize& size) -> void
    {
        pixmap_ = QPixmap{ size };
        reset();
    }

    auto reset() -> void
    {
        if (!pixmap_.isNull())
            pixmap_.fill();
        end_ = 0;
        wrapped_ = false;
    }

    auto empty() const noexcept -> bool
    {
        return pixmap_.isNull();
    }

    auto size() const noexcept -> QSize
    {
        return pixmap_.size();
    }

    template <std::invocable<Updater&> F>
    auto update(F&& f) -> void
    {
        auto updater = Updater{ *this };
        std::invoke(std::forward<F>(f), updater);
    }

    auto draw(QPainter& painter, const QPoint& top_left) -> void
    {
        if (wrapped_)
        {
            int head_width = end_;
            int tail_x = head_width;
            int tail_width = pixmap_.width() - end_;
            if (tail_width > 0)
                painter.drawPixmap(
                    top_left,
                    pixmap_,
                    QRect{ tail_x, 0, tail_width, pixmap_.height() });
            if (head_width > 0)
                painter.drawPixmap(
                    QPoint{ top_left.x() + tail_width, top_left.y() },
                    pixmap_,
                    QRect{ 0, 0, head_width, pixmap_.height() });
        }
        else
        {
            int head_width = end_;
            painter.drawPixmap(
                QPoint{ top_left.x() + pixmap_.width() - head_width,
                        top_left.y() },
                pixmap_,
                QRect{ 0, 0, head_width, pixmap_.height() });
        }
    }

private:
    QPixmap pixmap_;
    double end_;
    bool wrapped_;
    static constexpr double tol_ = 1e-5;
};

} // namespace plot::detail
