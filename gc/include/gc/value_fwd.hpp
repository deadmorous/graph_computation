/** @file
 * @brief Compatibility shim — redirects to mpk::mix::value Value fwd decls.
 *
 * Copyright (C) 2024-2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "mpk/mix/value/value_fwd.hpp"


namespace gc {

using mpk::mix::value::Value;
using mpk::mix::value::ValueSpan;
using mpk::mix::value::ConstValueSpan;

} // namespace gc
