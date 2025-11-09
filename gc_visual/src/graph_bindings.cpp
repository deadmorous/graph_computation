/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "gc_visual/graph_bindings.hpp"

#include <iostream>


namespace gc_visual {

auto operator<<(std::ostream& s, const ParamBinding& binding)
    -> std::ostream&
{
    return
        s << binding.input_name
          << binding.param_spec.path;
}

} // namespace gc_visual
