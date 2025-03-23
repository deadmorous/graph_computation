/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "gc/edge.hpp"


namespace gc {

auto operator<<(std::ostream& s, const EdgeInputEnd& ee)
    -> std::ostream&
{ return s << "I(" << ee.node << ',' << uint32_t{ee.port.v} << ')'; }

auto operator<<(std::ostream& s, const EdgeOutputEnd& ee)
    -> std::ostream&
{ return s << "O(" << ee.node << ',' << uint32_t{ee.port.v} << ')'; }

auto operator<<(std::ostream& s, const Edge& e)
    -> std::ostream&
{ return s << '[' << e.from << "->" << e.to << ']'; }

} // namespace gc
