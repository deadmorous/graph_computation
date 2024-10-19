#include "gc_visual/bitmap_view.hpp"

#include "gc_app/image.hpp"

#include <QPainter>

namespace {

auto to_qimage(const gc_app::Image& image)
    -> QImage
{
    auto result = QImage{QSize(image.size.width,
                               image.size.height),
                         QImage::Format_ARGB32};

    const auto* src = image.data.data();
    for (gc_app::Uint row=0; row<image.size.height; ++row)
    {
        auto* dst = result.scanLine(row);
        memcpy(dst, src, image.size.width*sizeof(uint32_t));
        src += image.size.width;
    }
    return result;
}

} // anonymous namespace


BitmapView::BitmapView(QWidget* parent) :
    QWidget{ parent }
{}

auto BitmapView::set_image(const gc::Value& image)
    -> void
{
    img_ = to_qimage(image.as<gc_app::Image>());
    update();
}

auto BitmapView::set_scale(double scale)
    -> void
{
    scale_ = scale;
    update();
}

auto BitmapView::paintEvent(QPaintEvent*)
    -> void
{
    auto p = QPainter{ this };
    p.scale(scale_, scale_);
    p.drawImage(QPoint{0, 0}, img_);
}
