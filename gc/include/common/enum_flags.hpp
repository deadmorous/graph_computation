/** @file
 * @brief Compatibility shim — redirects to mpk::mix::EnumFlags.
 *
 * Copyright (C) 2025-2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "common/enum_type.hpp"

#include "mpk/mix/enum_flags.hpp"


namespace common {

using mpk::mix::EnumFlags;
using mpk::mix::EnumFlagsType;
using mpk::mix::EnumFlagsOpArgType;

template <mpk::mix::EnumFlagsOpArgType T>
using EnumOf = mpk::mix::EnumOf<T>;

} // namespace common
