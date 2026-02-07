/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#pragma once

#include "common/unsafe.hpp"
#include "common/strong.hpp"
#include "common/strong_vector.hpp"

#include <QRect>

#include <cstdint>
#include <exception>

class QPainter;

namespace plot::layout {

GCLIB_STRONG_TYPE(ItemCount, uint32_t, common::StrongCountFeatures);
GCLIB_STRONG_TYPE(ItemIndex, uint32_t, common::StrongIndexFeatures<ItemCount>);

using ItemIndexRange = common::IndexRange<ItemIndex>;

constexpr inline auto central = ItemIndex{};

// ---

enum class Side : uint8_t
{
    Top,
    Right,
    Bottom,
    Left
};

enum class Anchor : uint8_t
{
    Top,
    TopRight,
    Right,
    BottomRight,
    Bottom,
    BottomLeft,
    Left,
    TopLeft
};

enum class Dimension : uint8_t
{
    Horizontal,
    Vertical
};

using ItemVec = common::StrongVector<QRect, ItemIndex>;

class LayoutOutOfSpaceException : public std::exception
{
public:
    LayoutOutOfSpaceException();

    auto what() const noexcept -> const char* override;
};

struct GridSpec final
{
    Anchor anchor;
    QPoint location;
    QSize available_size;
    Dimension growth_direction;
};

struct GridItemSpec final
{
    QSize size;
    QSize margin;
    QSize spacing;
};

struct Grid final
{
    QRect rect;
    ItemIndexRange items;
};

class Layout final
{
public:
    Layout();
    explicit Layout(const QRect& entire_area);

    auto items() const noexcept -> ItemIndexRange;

    auto rect() const noexcept -> const QRect&;

    auto rect(ItemIndex) const -> const QRect&;

    auto eat(Side, int size, ItemIndex from) -> ItemIndex;

    auto grid(const GridSpec&, const GridItemSpec&, ItemCount) -> Grid;

private:
    Layout(common::Unsafe_Tag, const QRect& entire_area);

    QRect entire_area_;
    ItemVec items_;
};

// ---

auto draw(const Layout&, QPainter&) -> void;

} // plot::layout
