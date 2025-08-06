#pragma once

#include <type_traits>


namespace agc_rt {

template <typename Value, typename Threshold>
auto threshold(Value v, Threshold t) noexcept
    -> bool
{ return v < t; }

} // namespace agc_rt
