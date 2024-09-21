#pragma once

#include "gc_app/image.hpp"

#include <QImage>
#include <QWidget>

class BitmapView
    : public QWidget
{
Q_OBJECT
public:
    explicit BitmapView(const gc_app::Image& image,
                        QWidget* parent = nullptr);

    auto image()
        -> QImage&;

protected:
    auto paintEvent(QPaintEvent*)
        -> void override;

private:
    QImage img_;
};
