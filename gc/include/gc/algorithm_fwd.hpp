/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024-2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "common/strong.hpp"

#include <cstddef>

namespace gc::alg {

using WeakId = size_t;

namespace id {

GCLIB_STRONG_TYPE(Assign, WeakId);
GCLIB_STRONG_TYPE(Block, WeakId);
GCLIB_STRONG_TYPE(Do, WeakId);
GCLIB_STRONG_TYPE(For, WeakId);
GCLIB_STRONG_TYPE(FuncInvocation, WeakId);
GCLIB_STRONG_TYPE(HeaderFile, WeakId);
GCLIB_STRONG_TYPE(If, WeakId);
GCLIB_STRONG_TYPE(InputBinding, WeakId);
GCLIB_STRONG_TYPE(Lib, WeakId);
GCLIB_STRONG_TYPE(OutputActivation, WeakId);
GCLIB_STRONG_TYPE(ReturnOutputActivation, WeakId);
GCLIB_STRONG_TYPE(Statement, WeakId);
GCLIB_STRONG_TYPE(Symbol, WeakId);
GCLIB_STRONG_TYPE(Type, WeakId);
GCLIB_STRONG_TYPE(TypeFromBinding, WeakId);
GCLIB_STRONG_TYPE(Var, WeakId);
GCLIB_STRONG_TYPE(Vars, WeakId);
GCLIB_STRONG_TYPE(While, WeakId);

} // namespace id

class AlgorithmStorage;

} // namespace gc::alg
