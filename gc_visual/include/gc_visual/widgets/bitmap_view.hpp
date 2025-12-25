/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#pragma once

#include "gc/value_fwd.hpp"

#include <QImage>
#include <QWidget>

class BitmapView
    : public QWidget
{
Q_OBJECT
public:
    enum class BlendMode : uint8_t
    {
        none,
        all,
        light,
        dark
    };

    explicit BitmapView(
        BlendMode blend_mode = BlendMode::none,
        double blend_factor = 0,
        QWidget* parent = nullptr);

    auto qimage() const -> QImage;

public slots:
    auto set_image(const gc::Value& image)
        -> void;

    auto set_scale(double scale)
        -> void;

    auto set_blend_factor(double blend_factor)
        -> void;

signals:
    void image_updated(const QImage&);

protected:
    auto paintEvent(QPaintEvent*)
        -> void override;

    auto mouseMoveEvent(QMouseEvent *event)
        -> void override;

private:
    QImage img_;
    double scale_{1};
    BlendMode blend_mode_{BlendMode::none};
    double blend_factor_{0};
};
