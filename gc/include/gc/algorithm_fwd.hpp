#pragma once

#include "common/strong.hpp"

#include <cstddef>

namespace gc::alg {

namespace id {

GCLIB_STRONG_TYPE(Block, size_t);
GCLIB_STRONG_TYPE(Do, size_t);
GCLIB_STRONG_TYPE(For, size_t);
GCLIB_STRONG_TYPE(FuncInvocation, size_t);
GCLIB_STRONG_TYPE(HeaderFile, size_t);
GCLIB_STRONG_TYPE(If, size_t);
GCLIB_STRONG_TYPE(InputBinding, size_t);
GCLIB_STRONG_TYPE(Lib, size_t);
GCLIB_STRONG_TYPE(OutputActivation, size_t);
GCLIB_STRONG_TYPE(Statement, size_t);
GCLIB_STRONG_TYPE(Symbol, size_t);
GCLIB_STRONG_TYPE(Type, size_t);
GCLIB_STRONG_TYPE(Var, size_t);
GCLIB_STRONG_TYPE(Vars, size_t);
GCLIB_STRONG_TYPE(While, size_t);

} // namespace id

class AlgorithmStorage;

} // namespace gc::alg
