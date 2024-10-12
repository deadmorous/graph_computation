#pragma once

#include "common/value_registry.h"


namespace common {

template <typename Interface, typename... ConstructionArgs>
using FactoryFunc = std::shared_ptr<Interface>(*)(ConstructionArgs...);


template <typename Interface, typename... ConstructionArgs>
using ObjectRegstry =
    ValueRegistry<FactoryFunc<Interface, ConstructionArgs...>>;

} // namespace common
