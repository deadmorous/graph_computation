/** @file
 * @brief Compatibility shim — redirects to mpk::mix::detail::SetLikeType.
 *
 * Copyright (C) 2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "mpk/mix/detail/set_like.hpp"


namespace common::detail {

using mpk::mix::detail::SetLikeType;

} // namespace common::detail
