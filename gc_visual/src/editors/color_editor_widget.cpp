/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024-2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "gc_visual/editors/color_editor_widget.hpp"

#include "gc_visual/color.hpp"

#include "gc/value.hpp"

#include <QColorDialog>
#include <QPainter>


ColorEditorWidget::ColorEditorWidget(const YAML::Node&, QWidget* parent) :
    ParameterEditorWidget{ parent }
{
    // TODO: Specify a better size policy
    setMinimumSize(48, 24);
    setMaximumSize(48, 24);

    setCursor(Qt::PointingHandCursor);
}

auto ColorEditorWidget::value() const -> gc::Value
{ return color_; }

auto ColorEditorWidget::check_type(const gc::Type* type) -> TypeCheckResult
{
    static auto expected_type = gc::type_of<gc_types::Color>();

    if (type == expected_type)
        return { .ok = true };

    return {
        .ok = false,
        .expected_type_description = common::format(expected_type)
    };
}

void ColorEditorWidget::set_value(const gc::Value& value)
{
    color_ = value.as<gc_types::Color>();
    update();
}

auto ColorEditorWidget::paintEvent(QPaintEvent*)
    -> void
{
    constexpr auto checker_size = 8;
    auto painter = QPainter{this};
    auto rc = rect();

    constexpr auto bg0 = gc_types::ColorComponent(0x54);
    constexpr auto bg1 = gc_types::ColorComponent(0xa8);

    auto c0 = gc_visual::qcolor(gc_types::blend_colors(
        gc_types::gray_color(bg0), color_));
    auto c1 = gc_visual::qcolor(gc_types::blend_colors(
        gc_types::gray_color(bg1), color_));

    for (int y=0, iy=0; y<rc.height(); y+=checker_size, ++iy)
        for (int x=0, ix=0; x<rc.width(); x+=checker_size, ++ix)
        {
            auto w = std::min(checker_size, rc.width()-x);
            auto h = std::min(checker_size, rc.height()-y);
            auto c = (ix+iy) & 1 ?  c1 : c0;
            painter.fillRect(x, y, w, h, c);
        }
    painter.setPen(QColor(0, 0, 0, 0x80));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(rc.adjusted(1, 0, 0, 0));
}

auto ColorEditorWidget::mouseReleaseEvent(QMouseEvent*)
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
    emit value_changed(color_);
}
