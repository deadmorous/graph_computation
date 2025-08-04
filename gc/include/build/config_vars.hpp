/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include <cstdint>
#include <string_view>


namespace build {

enum class ConfigType : uint8_t
{
    Debug,
    Release,
    RelWithDebInfo,
    MinSizeRel
};

extern const ConfigType current_config_type;

} // namespace build
