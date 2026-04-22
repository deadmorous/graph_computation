/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#pragma once

#include "gc/port.hpp"
#include "gc/value_fwd.hpp"

#include "mpk/mix/strong/span.hpp"

#include <string_view>


namespace gc {

using InputNames =
    mpk::mix::StrongSpan<const std::string_view, InputPort>;

using OutputNames =
    mpk::mix::StrongSpan<const std::string_view, OutputPort>;

using OutputValues =
    mpk::mix::StrongSpan<Value, OutputPort>;

using InputValues =
    mpk::mix::StrongSpan<Value, InputPort>;

using ConstInputValues =
    mpk::mix::StrongSpan<const Value, InputPort>;

} // namespace gc
