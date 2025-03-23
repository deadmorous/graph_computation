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

#include "common/value_registry.hpp"


namespace common {

template <typename Interface, typename... ConstructionArgs>
using FactoryFunc = std::shared_ptr<Interface>(*)(ConstructionArgs...);


template <typename Interface, typename... ConstructionArgs>
using ObjectRegistry =
    ValueRegistry<FactoryFunc<Interface, ConstructionArgs...>>;

} // namespace common
