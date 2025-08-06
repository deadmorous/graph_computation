#pragma once

#include <iostream>


namespace agc_rt {

template <typename T>
auto print(const T& value)
    -> void
{ std::cout << value << std::endl; }

} // namespace agc_rt
