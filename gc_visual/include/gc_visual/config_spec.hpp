/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include <cstdint>
#include <QString>

namespace gc_visual {

enum class ConfigSpecificationType : uint8_t
{
    Content,
    FileName
};

struct ConfigSpecification
{
    ConfigSpecificationType type;
    QString spec;
};

} // namespace gc_visual
