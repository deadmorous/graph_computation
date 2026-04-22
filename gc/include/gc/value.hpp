/** @file
 * @brief Compatibility shim — redirects to mpk::mix::value::Value.
 *
 * Copyright (C) 2024-2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "gc/value_fwd.hpp"
#include "gc/type.hpp"
#include "gc/value_path.hpp"

#include "mpk/mix/util/throw.hpp"
#include "mpk/mix/meta/type.hpp"

#include "mpk/mix/value/value.hpp"


namespace gc {

using mpk::mix::value::ValueVec;

} // namespace gc
