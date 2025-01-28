#include "common/detail/ind.hpp"

namespace common::detail {

auto operator<<(std::ostream& s, Ind ind)
    -> std::ostream&
{
    for (Ind::Weak i=0, n=tab_size*ind.v; i<n; ++i)
        s << ' ';
    return s;
}

auto next(Ind ind)
    -> Ind
{ return ind + Ind{1u}; }

} // namespace common::detail
