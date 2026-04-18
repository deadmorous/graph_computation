/** @file
 * @brief Compatibility shim — use mpk/mix/value/parse_simple_value.hpp directly
 *        in new code.
 *
 * Copyright (C) 2024-2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "mpk/mix/value/parse_simple_value.hpp"


namespace gc {

using mpk::mix::value::parse_simple_value;

} // namespace gc
