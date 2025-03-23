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
    explicit BitmapView(QWidget* parent = nullptr);

public slots:
    auto set_image(const gc::Value& image)
        -> void;

    auto set_scale(double scale)
        -> void;

protected:
    auto paintEvent(QPaintEvent*)
        -> void override;

    auto mouseMoveEvent(QMouseEvent *event)
        -> void override;

private:
    QImage img_;
    double scale_{1};
};
