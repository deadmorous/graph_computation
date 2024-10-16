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

protected:
    auto paintEvent(QPaintEvent*)
        -> void override;

private:
    QImage img_;
};
