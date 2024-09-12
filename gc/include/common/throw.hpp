#pragma once

#include "common/format.hpp"

#include <stdexcept>


namespace common {

template <typename exception = std::runtime_error, typename... Ts>
[[noreturn]]
auto throw_(Ts&&... args)
    -> void
{ throw exception(format(std::forward<Ts>(args)...)); }

} // namespace common
