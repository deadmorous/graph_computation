#pragma once
// Compatibility shim — use mpk/mix/value/object_registry.hpp directly in new code.
#include "mpk/mix/value/object_registry.hpp"

namespace common
{
using mpk::mix::value::FactoryFunc;
using mpk::mix::value::ObjectRegistry;
} // namespace common
