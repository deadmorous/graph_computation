#include "gc_visual/bitmap_view.hpp"

#include "gc_visual/qstr.hpp"

#include "gc_app/image.hpp"

#include <QMouseEvent>
#include <QPainter>
#include <QToolTip>

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
{
    setMouseTracking(true);
}

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

auto BitmapView::mouseMoveEvent(QMouseEvent *event)
    -> void
{
    auto globalPos = event->globalPosition().toPoint();

    auto pos = event->pos();
    auto x = static_cast<int>(pos.x() / scale_);
    auto y = static_cast<int>(pos.y() / scale_);
    auto text = QString{};
    if (x < img_.width() && y < img_.height())
    {
        auto index = x + y*img_.width();
        text = format_qstr("index = ", index, "\npos=(", x, ", ", y, ")");
    }

    QToolTip::showText(globalPos, text, this, {}, 5000);
}
