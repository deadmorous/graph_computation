/** @file
 * @brief Compatibility shim — use mpk/mix/util/pow2.hpp directly in new code.
 *
 * Copyright (C) 2024 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "common/type.hpp"
#include "mpk/mix/util/pow2.hpp"


namespace common {

using mpk::mix::floor2;
using mpk::mix::ceil2;

} // namespace common
