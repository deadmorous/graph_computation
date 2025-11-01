/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "gc_app/types/color.hpp"

#include <QWidget>


class ColorEditorWidget final :
    public QWidget
{
    Q_OBJECT

public:
    explicit ColorEditorWidget(gc_app::Color color, QWidget* parent = nullptr);

    auto color() const noexcept -> gc_app::Color;

signals:
    auto valueChanged(gc_app::Color color) -> void;

public slots:
    auto setColor(gc_app::Color color) -> void;

protected:
    auto paintEvent(QPaintEvent*)
        -> void override;

    auto mouseReleaseEvent(QMouseEvent *event)
        -> void override;

private:
    gc_app::Color color_;
};
