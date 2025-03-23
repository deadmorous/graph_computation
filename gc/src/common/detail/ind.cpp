/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

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

ScopedInd::ScopedInd(Ind& ind):
    ind_{ ind }
{
    ++ind_.v;
}

ScopedInd::~ScopedInd()
{
    --ind_.v;
}

} // namespace common::detail
