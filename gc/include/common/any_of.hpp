#pragma once

#include <concepts>

namespace common {

template <typename T, typename... Ts>
concept AnyOf = (std::same_as<T, Ts> || ...);

} // namespace common
