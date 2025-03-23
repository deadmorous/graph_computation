/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include <span>

namespace gc {

class Value;

using ValueSpan = std::span<Value>;
using ConstValueSpan = std::span<const Value>;

} // namespace gc
