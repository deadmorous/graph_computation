/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "common/struct_type_macro.hpp"

#include <cstdint>
#include <vector>


namespace gc_app {

struct Cell2dRules final
{
    uint8_t state_count{2};
    int8_t min_state{0};
    bool tor{true};
    bool count_self{false};
    std::vector<int8_t> map9{ 0, 0, -128, 1, 0, 0, 0, 0, 0 };
    std::vector<int8_t> map6{ 0, 0, -128, 1, 0, 0 };
    std::vector<int8_t> map4{ 0, 0, -128, 1 };

    auto operator==(const Cell2dRules&) const noexcept -> bool = default;
};
GCLIB_STRUCT_TYPE(
    Cell2dRules,
    state_count, min_state, tor, count_self, map9, map6, map4);

} // namespace gc_app
