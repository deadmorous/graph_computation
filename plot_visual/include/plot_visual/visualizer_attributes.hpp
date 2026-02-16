/** @file
 * @brief Common attributes for plot visualizers.
 *
 * Copyright (C) 2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#pragma once

#include "gc_types/palette.hpp"

#include <QString>

namespace plot {

struct VisualizerAttributes final
{
    gc_types::IndexedPalette palette;
    QString x_label;
    QString y_label;
    QString title;
};

} // namespace plot
