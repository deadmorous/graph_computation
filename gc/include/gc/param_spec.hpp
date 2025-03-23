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

#include "gc/computation_node_fwd.hpp"
#include "gc/value_path.hpp"

namespace gc {

struct ParameterSpec
{
    size_t          input;
    gc::ValuePath   path;
};

} // namespace gc
