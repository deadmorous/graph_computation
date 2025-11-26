/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024-2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "gc/value_path.hpp"
#include "gc/edge.hpp"

#include <variant>

namespace gc {

struct ExternalInputSpec final
{
    size_t input;
};

struct NodeOutputSpec final
{
    EdgeOutputEnd output;
};

using IoSpec = std::variant<ExternalInputSpec, NodeOutputSpec>;

struct ParameterSpec final
{
    IoSpec io;
    gc::ValuePath   path;
};

} // namespace gc
