/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "common/pow2.hpp"

#include <gtest/gtest.h>

TEST(Common_Pow2, Floor)
{
    static_assert(common::floor2(0u) == 0);
    static_assert(common::floor2(1u) == 1);
    static_assert(common::floor2(64u) == 64);
    static_assert(common::floor2(100u) == 64);
}

TEST(Common_Pow2, Ceil)
{
    static_assert(common::ceil2(0u) == 0);
    static_assert(common::ceil2(1u) == 1);
    static_assert(common::ceil2(64u) == 64);
    static_assert(common::ceil2(100u) == 128);
}
