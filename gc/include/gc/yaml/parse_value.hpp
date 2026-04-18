/** @file
 * @brief Compatibility shim — use mpk/mix/serial/yaml/parse_value.hpp directly
 *        in new code.
 *
 * Copyright (C) 2024-2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "gc/type_registry.hpp"
#include "gc/value_fwd.hpp"

#include "mpk/mix/serial/yaml/parse_value.hpp"


namespace gc::yaml {

using mpk::mix::serial::yaml::parse_value;

} // namespace gc::yaml
