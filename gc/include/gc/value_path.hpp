/** @file
 * @brief Compatibility shim — redirects to mpk::mix::value value path types.
 *
 * Copyright (C) 2024-2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "mpk/mix/value/value_path.hpp"


namespace gc {

using mpk::mix::value::ValuePathItem;
using mpk::mix::value::ValuePathView;
using mpk::mix::value::ValuePath;
using mpk::mix::value::ValuePathLikeType;

using mpk::mix::value::operator/;

} // namespace gc
