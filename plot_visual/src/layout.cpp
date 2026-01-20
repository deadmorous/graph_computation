/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#include "plot_visual/layout.hpp"

#include <QPainter>

#include <cassert>

namespace layout {

namespace {

auto checked_rect(const QRect& rect) -> QRect
{
    auto result = rect.normalized();
    if (result.width() <= 0 || result.height() <= 0)
        throw LayoutOutOfSpaceException{};
    return result;
}

} // anonymous namespace

LayoutOutOfSpaceException::LayoutOutOfSpaceException() = default;

auto LayoutOutOfSpaceException::what() const noexcept -> const char*
{
    return "No space left in  the layout";
}

// ---

Layout::Layout() = default;

Layout::Layout(const QRect& entire_area):
    Layout{common::Unsafe, checked_rect(entire_area)}
{}

Layout::Layout(common::Unsafe_Tag, const QRect& entire_area):
    entire_area_{entire_area},
    items_{entire_area}
{}

auto Layout::items() const noexcept -> ItemIndexRange
{
    return items_.index_range();
}

auto Layout::rect() const noexcept -> const QRect&
{
    return entire_area_;
}

auto Layout::rect(ItemIndex index) const -> const QRect&
{
    return items_.at(index);
}

auto Layout::eat(Side side, int size, ItemIndex from)
    -> ItemIndex
{
    if (size <= 0)
        return from;

    // NOTE: Should not be accessing after adding to items_
    auto& donor = items_.at(from);

    auto result = ItemIndex{0} + items_.size();
    switch(side)
    {
    case Side::Top:
    {
        if (donor.height() <= size)
            throw LayoutOutOfSpaceException{};
        auto top = donor.top();
        donor.adjust(0, size, 0, 0);
        items_.emplace_back(
            donor.left(), top, donor.width(), size);
        return result;
    }
    case Side::Right:
        if (donor.width() <= size)
            throw LayoutOutOfSpaceException{};
        donor.adjust(0, 0, -size, 0);
        items_.emplace_back(
            donor.right(), donor.top(), size, donor.height());
        return result;
    case Side::Bottom:
        if (donor.height() <= size)
            throw LayoutOutOfSpaceException{};
        donor.adjust(0, 0, 0, -size);
        items_.emplace_back(
            donor.left(), donor.bottom(), donor.width(), size);
        return result;
    case Side::Left:
    {
        if (donor.width() <= size)
            throw LayoutOutOfSpaceException{};
        auto left = donor.left();
        donor.adjust(size, 0, 0, 0);
        items_.emplace_back(
            left, donor.top(), size, donor.height());
        return result;
    }
    }
    __builtin_unreachable();
}

auto Layout::grid(const GridSpec& grid_spec,
                  const GridItemSpec& item_spec,
                  ItemCount item_count) -> Grid
{
    auto max_count = [](int avail, int size, int margin, int spacing)
    {
        const auto _2m = 2*margin;
        if (avail <= _2m)
            throw LayoutOutOfSpaceException{};

        auto result = (avail - _2m + spacing) / (size + spacing);
        if (result <= 0)
            throw LayoutOutOfSpaceException{};
        return result;
    };

    auto max_h = max_count(grid_spec.available_size.width(),
                           item_spec.size.width(),
                           item_spec.margin.width(),
                           item_spec.spacing.width());
    auto max_v = max_count(grid_spec.available_size.height(),
                           item_spec.size.height(),
                           item_spec.margin.height(),
                           item_spec.spacing.height());
    if (max_h * static_cast<size_t>(max_v) < item_count.v)
        throw LayoutOutOfSpaceException{};

    switch (grid_spec.growth_direction)
    {
    case Dimension::Horizontal:
        break;
    case Dimension::Vertical:
        break;
    }

    // TODO
    return {
        .rect={},
        .items=common::index_range(ItemIndex{0}, ItemIndex{0})
    };
}

auto draw(const Layout& layout, QPainter& painter) -> void
{
    painter.drawRect(layout.rect());
    for (auto index : layout.items())
        painter.drawRect(layout.rect(index));
}

} // layout
