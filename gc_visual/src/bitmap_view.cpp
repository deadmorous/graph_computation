#include "gc_visual/bitmap_view.hpp"

#include <QPainter>


#if 0 // TODO
BitmapView::BitmapView(SequenceGeneratorInterface& gen,
                       PresentationInterface& presentation,
                       QWidget* parent)
    : QWidget{ parent }
    , gen_{ gen }
    , presentation_{ presentation }
{
    setMinimumSize(200, 200);
}

auto BitmapView::image()
    -> QImage&
{ return img_; }

auto BitmapView::paintEvent(QPaintEvent*)
    -> void
{
    presentation_.represent(img_, gen_);
    auto p = QPainter{ this };
    p.drawImage(QPoint{0, 0}, img_);
}

#endif // 0
