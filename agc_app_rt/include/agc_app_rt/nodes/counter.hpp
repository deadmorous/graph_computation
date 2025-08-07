#pragma once

#include <cstdint>


namespace agc_app_rt {

inline auto reset_counter(uint64_t& counter) noexcept
    -> void
{ counter = 0; }

inline auto next_counter(uint64_t& counter) noexcept
    -> void
{ ++counter; }

} // namespace agc_app_rt
