/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "common/any_of.hpp"

namespace gc {

constexpr inline struct Input_Tag final {} Input;
constexpr inline struct Output_Tag final {} Output;

template <typename T>
concept PortTagType = common::AnyOf<T, Input_Tag, Output_Tag>;

} // namespace gc
