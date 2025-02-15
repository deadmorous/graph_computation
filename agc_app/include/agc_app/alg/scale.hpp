#pragma once

#include <type_traits>


namespace agc_app {

template <typename Factor, typename Value>
auto scale(Factor f, Value v) noexcept
    -> std::common_type_t<Factor, Value>
{ return f * v; }

} // namespace agc_app
