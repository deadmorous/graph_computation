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


namespace build {

enum class OutputType : uint8_t
{
    Executable,
    SharedObject
};

} // namespace build
