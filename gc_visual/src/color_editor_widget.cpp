#include "gc_visual/color_editor_widget.hpp"

#include "gc_visual/color.hpp"

#include <QColorDialog>
#include <QPainter>


ColorEditorWidget::ColorEditorWidget(gc_app::Color color,
                                     QWidget* parent) :
    QWidget{ parent },
    color_{ color }
{
    // TODO: Specify a better size policy
    setMinimumSize(48, 24);
    setMaximumSize(48, 24);

    setCursor(Qt::PointingHandCursor);
}

auto ColorEditorWidget::color() const noexcept -> gc_app::Color
{ return color_; }

auto ColorEditorWidget::setColor(gc_app::Color color)
    -> void
{
    color_ = color;
    update();
}

auto ColorEditorWidget::paintEvent(QPaintEvent*)
    -> void
{
    constexpr auto checker_size = 8;
    auto painter = QPainter{this};
    auto rc = rect();

    constexpr auto bg0 = gc_app::ColorComponent(0x54);
    constexpr auto bg1 = gc_app::ColorComponent(0xa8);

    auto c0 = gc_visual::qcolor(gc_app::blend_colors(
        gc_app::gray_color(bg0), color_));
    auto c1 = gc_visual::qcolor(gc_app::blend_colors(
        gc_app::gray_color(bg1), color_));

    for (int y=0, iy=0; y<rc.height(); y+=checker_size, ++iy)
        for (int x=0, ix=0; x<rc.width(); x+=checker_size, ++ix)
        {
            auto w = std::min(checker_size, rc.width()-x);
            auto h = std::min(checker_size, rc.height()-y);
            auto c = (ix+iy) & 1 ?  c1 : c0;
            painter.fillRect(x, y, w, h, c);
        }
    painter.setPen(QColor(0, 0, 0, 0xc7));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(rc.adjusted(0, 0, -1, -1));
}

auto ColorEditorWidget::mouseReleaseEvent(QMouseEvent *event)
    -> void
{
    auto color = gc_visual::qcolor(color_);
    auto new_qcolor = QColorDialog::getColor(
        color,
        this,
        tr("&Pick color"),
        QColorDialog::ShowAlphaChannel | QColorDialog::DontUseNativeDialog);
    if (!new_qcolor.isValid())
        return;
    color_ = gc_visual::color(new_qcolor);
    emit valueChanged(color_);
}
