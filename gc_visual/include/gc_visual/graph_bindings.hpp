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
    std::vector<gc::EdgeInputEnd> inputs;
};

struct ParamBinding final
{
    gc::ParameterSpec   param_spec;
    std::string         io_name;
};

auto operator<<(std::ostream& s, const ParamBinding& binding)
    -> std::ostream&;

struct OutputBinding final
{
    gc::EdgeOutputEnd output;
};

} // namespace gc_visual
