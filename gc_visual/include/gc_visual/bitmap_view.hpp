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

private:
    QImage img_;
    double scale_{1};
};
