#pragma once

#include "common/value_registry.hpp"


namespace common {

template <typename Interface, typename... ConstructionArgs>
using FactoryFunc = std::shared_ptr<Interface>(*)(ConstructionArgs...);


template <typename Interface, typename... ConstructionArgs>
using ObjectRegistry =
    ValueRegistry<FactoryFunc<Interface, ConstructionArgs...>>;

} // namespace common
