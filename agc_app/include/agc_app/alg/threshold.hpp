#pragma once

#include <type_traits>


namespace agc_app {

template <typename Value, typename Threshold>
auto threshold(Value v, Threshold t) noexcept
    -> bool
{ return v < t; }

} // namespace agc_app
