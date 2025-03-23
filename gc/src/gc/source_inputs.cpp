/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "gc/source_inputs.hpp"

namespace gc {

auto operator<<(std::ostream& s, const SourceInputs& source_inputs)
    -> std::ostream&
{
    auto n = source_inputs.values.size();

    if (group_count(source_inputs.destinations) != n)
        return s << "<invalid SourceInputs>";

    s << '{';
    auto delim = "";
    for (size_t i=0; i<n; ++i, delim = ", ")
    {
        auto destinations = group(source_inputs.destinations, i);
        s << delim
          << source_inputs.values[i] << " -> ["
          << common::format_seq(destinations)
          << "]";
    }
    s << '}';

    return s;
}

} // namespace gc
