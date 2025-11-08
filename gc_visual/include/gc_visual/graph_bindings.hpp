/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "gc/edge.hpp"
#include "gc/param_spec.hpp"


namespace gc_visual {

struct InputBinding final
{
    size_t          input;
};

struct ParamBinding final
{
    gc::ParameterSpec   param_spec;
    std::string         input_name;
};

struct OutputBinding final
{
    gc::EdgeOutputEnd output;
};

} // namespace gc_visual
