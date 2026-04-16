/** @file
 * @brief Compatibility shim — redirects to mpk::mix::value::detail.
 *
 * Copyright (C) 2024-2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "mpk/mix/value/detail/value_component_access.hpp"


namespace gc::detail {

using mpk::mix::value::detail::actually_equality_comparable;
using mpk::mix::value::detail::ValueComponentAccess;
using mpk::mix::value::detail::ValueComponentsAccessFactoryFunc;
using mpk::mix::value::detail::value_components_access_factory;
using mpk::mix::value::detail::ValueComponents;

} // namespace gc::detail
