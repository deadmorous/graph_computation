#pragma once

#include "common/strong.hpp"

#include <cstddef>

namespace agc {

namespace id {

GCLIB_STRONG_TYPE(Var, size_t);
GCLIB_STRONG_TYPE(Vars, size_t);
GCLIB_STRONG_TYPE(Func, size_t);
GCLIB_STRONG_TYPE(InputBinding, size_t);
GCLIB_STRONG_TYPE(FuncInvocation, size_t);
GCLIB_STRONG_TYPE(OutputActivation, size_t);
GCLIB_STRONG_TYPE(If, size_t);
GCLIB_STRONG_TYPE(For, size_t);
GCLIB_STRONG_TYPE(While, size_t);
GCLIB_STRONG_TYPE(Do, size_t);
GCLIB_STRONG_TYPE(Block, size_t);
GCLIB_STRONG_TYPE(Statement, size_t);

} // namespace id

class AlgorithmStorage;

} // namespace agc
