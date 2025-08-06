/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include <iostream>


namespace agc_rt {

template <typename T>
auto print(const T& value)
    -> void
{ std::cout << value << std::endl; }

} // namespace agc_rt
