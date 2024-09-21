#include "gc_visual/bitmap_view.hpp"

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


BitmapView::BitmapView(const gc_app::Image& image,
                       QWidget* parent) :
    QWidget{ parent },
    img_{ to_qimage(image) }
{
    // TODO
    setMinimumSize(200, 200);
}

auto BitmapView::image()
    -> QImage&
{ return img_; }

auto BitmapView::paintEvent(QPaintEvent*)
    -> void
{
    auto p = QPainter{ this };
    p.drawImage(QPoint{0, 0}, img_);
}
