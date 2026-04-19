/** @file
 * @brief Compatibility shim — use mpk/mix/strong/grouped.hpp directly in new code.
 *
 * Copyright (C) 2024 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#pragma once

#include "common/format.hpp"
#include "mpk/mix/strong/grouped.hpp"

namespace common {

using mpk::mix::Grouped;
using mpk::mix::add_to_last_group;
using mpk::mix::next_group;
using mpk::mix::group_count;
using mpk::mix::group_indices;
using mpk::mix::group;

} // namespace common
