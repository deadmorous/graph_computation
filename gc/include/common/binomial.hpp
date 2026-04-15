/** @file
 * @brief Compatibility shim — use mpk/mix/util/binomial.hpp directly in new code.
 *
 * Copyright (C) 2024 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "common/type.hpp"
#include "mpk/mix/util/binomial.hpp"


namespace common {

using mpk::mix::binomial;

} // namespace common
