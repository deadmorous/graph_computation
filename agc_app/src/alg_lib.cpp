/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "agc_app/alg_lib.hpp"

#include "gc/algorithm.hpp"


namespace agc_app {

auto alg_lib(gc::alg::AlgorithmStorage& s)
    -> gc::alg::id::Lib
{ return s(gc::alg::Lib{ .name = "agc_app" }); }

} // namespace agc_app
