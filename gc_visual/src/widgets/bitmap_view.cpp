/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024-2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#include "gc_visual/widgets/bitmap_view.hpp"

#include "gc_visual/qstr.hpp"

#include "gc_types/image.hpp"

#include "common/const.hpp"

#include <QMouseEvent>
#include <QPainter>
#include <QToolTip>

namespace {

using BlendMode = BitmapView::BlendMode;

auto to_qimage(const gc_types::ColorImage& image)
    -> QImage
{
    auto result = QImage{QSize(image.size.width,
                               image.size.height),
                         QImage::Format_ARGB32};

    const auto* src = image.data.data();
    for (gc_types::Uint row=0; row<image.size.height; ++row)
    {
        auto* dst = result.scanLine(row);
        memcpy(dst, src, image.size.width*sizeof(uint32_t));
        src += image.size.width;
    }
    return result;
}

struct Rational
{
    long num;
    long denom;
    Rational(double x, double scale=1000) :
        num{std::lround(x*scale)},
        denom{std::lround(scale)}
    {}

    auto multiply(int x) const -> int
    {
        return x * num / denom;
    }
};

auto interp_color_component(int c0, int c1, const Rational& f) -> int
{
    return c1 + f.multiply(c0-c1);
}

auto is_light(QRgb c) -> bool
{
    constexpr auto component_threshold = 255 / 2;
    auto r = qRed(c);
    auto g = qGreen(c);
    auto b = qBlue(c);
    auto result =
        r > component_threshold ||
        g > component_threshold ||
        b > component_threshold;
    return result;
}

auto interp_color(
    common::Const_Tag<BlendMode::all>,
    QRgb c0, QRgb c1, const Rational& f) -> QRgb
{
    auto r = interp_color_component(qRed(c0), qRed(c1), f);
    auto g = interp_color_component(qGreen(c0), qGreen(c1), f);
    auto b = interp_color_component(qBlue(c0), qBlue(c1), f);
    auto a = interp_color_component(qAlpha(c0), qAlpha(c1), f);
    return qRgba(r, g, b, a);
}

auto interp_color(
    common::Const_Tag<BlendMode::light>,
    QRgb c0, QRgb c1, const Rational& f) -> QRgb
{
    if (!is_light(c1))
        return c1;

    return interp_color(common::Const<BlendMode::all>, c0, c1, f);
}

auto interp_color(
    common::Const_Tag<BlendMode::dark>,
    QRgb c0, QRgb c1, const Rational& f) -> QRgb
{
    if (is_light(c1))
        return c1;

    return interp_color(common::Const<BlendMode::all>, c0, c1, f);
}

template <BlendMode mode>
auto blend_impl(common::Const_Tag<mode> mode_tag,
                const QImage& img0,
                QImage img1,
                double factor)
    -> QImage
{
    auto size = img1.size();
    if (factor == 0 || img0.isNull() || img0.size() != size)
        return img1;
    auto result = QImage{size, QImage::Format_ARGB32};

    auto width = static_cast<size_t>(size.width());
    auto height = static_cast<size_t>(size.height());
    auto f = Rational{factor};
    for (gc_types::Uint row=0; row<height; ++row)
    {
        const auto* src0 = reinterpret_cast<const QRgb*>(img0.scanLine(row));
        const auto* src1 = reinterpret_cast<const QRgb*>(img1.scanLine(row));
        auto* dst = reinterpret_cast<QRgb*>(result.scanLine(row));
        for (auto col=0u; col<width; ++col, ++src0, ++src1, ++dst)
            *dst = interp_color(mode_tag, *src0, *src1, f);
    }

    return result;
}

auto blend(const QImage& img0, QImage img1, BlendMode mode, double factor)
    -> QImage
{
    switch(mode)
    {
    case BlendMode::none:
        return img1;
    case BlendMode::all:
        return blend_impl(common::Const<BlendMode::all>, img0, img1, factor);
    case BlendMode::light:
        return blend_impl(common::Const<BlendMode::light>, img0, img1, factor);
    case BlendMode::dark:
        return blend_impl(common::Const<BlendMode::dark>, img0, img1, factor);
    }
    __builtin_unreachable();
}

} // anonymous namespace


BitmapView::BitmapView(BlendMode blend_mode,
                       double blend_factor,
                       QWidget* parent) :
    QWidget{ parent },
    blend_mode_{blend_mode},
    blend_factor_{blend_factor}
{
    setMouseTracking(true);
}

auto BitmapView::qimage() const -> QImage
{
    return img_;
}

auto BitmapView::set_image(const gc::Value& image)
    -> void
{
    img_ = blend(
        img_,
        to_qimage(image.as<gc_types::ColorImage>()),
        blend_mode_,
        blend_factor_);
    resize(img_.size() * scale_);
    update();
    emit image_updated(img_);
}

auto BitmapView::set_scale(double scale)
    -> void
{
    scale_ = scale;
    resize(img_.size() * scale_);
    update();
}

auto BitmapView::set_blend_factor(double blend_factor)
    -> void
{
    blend_factor_ = blend_factor;
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
