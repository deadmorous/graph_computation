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

#include "gc_visual/plot/concepts.hpp"

#include <QString>

#include <vector>

namespace plot {

template <CoordinateMappingType CoordinateMapping>
class TickBuf final
{
public:
    using From = typename CoordinateMapping::From;
    using FromVec = std::vector<From>;
    FromVec primary = FromVec(11);
    FromVec secondary = FromVec(51);
};

template <CoordinateMappingType CoordinateMapping_>
struct Axis final
{
public:
    using CoordinateMapping = CoordinateMapping_;

    CoordinateMapping mapping;
    QString label;
    bool show_primary_grid{true};
    bool show_secondary_grid{false};
    bool show_primary_tick_marks{false};
    bool show_secondary_tick_marks{false};

    mutable TickBuf<CoordinateMapping> tick_buf;

    auto primary_ticks() const noexcept
    {
        return mapping.ticks(tick_buf.primary);
    }

    auto secondary_ticks() const noexcept
    {
        return mapping.ticks(tick_buf.secondary);
    }
};

} // namespace plot
