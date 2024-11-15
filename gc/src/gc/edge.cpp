#include "gc/edge.hpp"


namespace gc {

auto operator<<(std::ostream& s, const EdgeEnd& ee)
    -> std::ostream&
{ return s << '(' << ee.node << ',' << ee.port << ')'; }

auto operator<<(std::ostream& s, const Edge& e)
    -> std::ostream&
{ return s << '[' << e[0] << "->" << e[1] << ']'; }

} // namespace gc
