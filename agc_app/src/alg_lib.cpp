#include "agc_app/alg_lib.hpp"

#include "gc/algorithm.hpp"


namespace agc_app {

auto alg_lib(gc::alg::AlgorithmStorage& s)
    -> gc::alg::id::Lib
{ return s(gc::alg::Lib{ .name = "agc_app" }); }

} // namespace agc_app
