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

#include "mpk/mix/strong/strong.hpp"

#include <cstddef>

namespace gc::alg {

using WeakId = size_t;

namespace id {

MPKMIX_STRONG_TYPE(Assign, WeakId);
MPKMIX_STRONG_TYPE(Block, WeakId);
MPKMIX_STRONG_TYPE(Do, WeakId);
MPKMIX_STRONG_TYPE(For, WeakId);
MPKMIX_STRONG_TYPE(FuncInvocation, WeakId);
MPKMIX_STRONG_TYPE(HeaderFile, WeakId);
MPKMIX_STRONG_TYPE(If, WeakId);
MPKMIX_STRONG_TYPE(InputBinding, WeakId);
MPKMIX_STRONG_TYPE(Lib, WeakId);
MPKMIX_STRONG_TYPE(OutputActivation, WeakId);
MPKMIX_STRONG_TYPE(ReturnOutputActivation, WeakId);
MPKMIX_STRONG_TYPE(Statement, WeakId);
MPKMIX_STRONG_TYPE(Symbol, WeakId);
MPKMIX_STRONG_TYPE(Type, WeakId);
MPKMIX_STRONG_TYPE(TypeFromBinding, WeakId);
MPKMIX_STRONG_TYPE(Var, WeakId);
MPKMIX_STRONG_TYPE(Vars, WeakId);
MPKMIX_STRONG_TYPE(While, WeakId);

} // namespace id

class AlgorithmStorage;

} // namespace gc::alg
