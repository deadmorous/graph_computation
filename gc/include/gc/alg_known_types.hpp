#pragma once

#include "gc/algorithm_fwd.hpp"
#include "gc/alg_type_literal.hpp"
#include "gc/decl_literal.hpp"


namespace gc::alg {

GCLIB_DECL_LITERAL(TypeLiteral, int_type);
GCLIB_DECL_LITERAL(TypeLiteral, uint64_t_type);
GCLIB_DECL_LITERAL(TypeLiteral, double_type);
GCLIB_DECL_LITERAL(TypeLiteral, size_t_type);

auto well_known_type(TypeLiteral, AlgorithmStorage&)
    -> id::Type;

} // namespace gc::alg
