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

namespace plot::painter::detail {

class XIncrementalDrawing final
{
public:
    class Updater final
    {
    friend class XIncrementalDrawing;
    public:
        template <std::invocable<QPainter&, double> F>
        auto operator()(double width, bool clip, F&& update) -> void
        {
            auto x = drawing_.end_;
            if (x + width > drawing_.size_.width() + drawing_.tol_)
            {
                x = 0;
                drawing_.wrapped_ = true;
            }
            drawing_.end_ = x + width;
            if (clip)
            {
                painter_.setClipRect(QRectF{
                    x,
                    0.,
                    width,
                    static_cast<double>(drawing_.size_.height())});
            }
            std::invoke(std::forward<F>(update), painter_, x);

            // Uncomment to debug
            // painter_.fillRect(x, 0, 2, drawing_.pixmap_.height(), Qt::gray);
            // painter_.setPen(Qt::black);
            // painter_.drawText(
            //     QPointF{x, 20.}, QString::number(static_cast<int>(x)));
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

    auto resize(const QSize& size, double dpr) -> void
    {
        pixmap_ = QPixmap{ size * dpr };
        pixmap_.setDevicePixelRatio(dpr);
        size_ = size;
        dpr_ = dpr;
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
    { return pixmap_.isNull(); }

    auto size() const noexcept -> QSize
    { return size_; }

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
            int tail_width = size_.width() - end_;
            if (tail_width > 0)
                painter.drawPixmap(
                    top_left,
                    pixmap_,
                    QRectF{ tail_x * dpr_,
                            0.,
                            tail_width * dpr_,
                            static_cast<double>(pixmap_.height()) });
            if (head_width > 0)
                painter.drawPixmap(
                    QPoint{ top_left.x() + tail_width, top_left.y() },
                    pixmap_,
                    QRectF{ 0.,
                            0.,
                            head_width * dpr_,
                            static_cast<double>(pixmap_.height()) });
        }
        else
        {
            int head_width = end_;
            painter.drawPixmap(
                QPoint{ top_left.x() + size_.width() - head_width,
                        top_left.y() },
                pixmap_,
                QRectF{ 0., 0., head_width * dpr_,
                        static_cast<double>(pixmap_.height()) });
        }
    }

private:
    QPixmap pixmap_;
    QSize size_;
    double dpr_;
    double end_;
    bool wrapped_;
    static constexpr double tol_ = 1e-5;
};

} // namespace plot::painter::detail
