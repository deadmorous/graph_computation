/** @file
 * @brief Compatibility shim — use mpk/mix/value/value_registry.hpp directly in
 *        new code.
 *
 * Copyright (C) 2024-2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "mpk/mix/value/value_registry.hpp"


namespace common {

template <typename T>
using ValueRegistry = mpk::mix::value::ValueRegistry<T>;

} // namespace common
