/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "gc/algorithm.hpp"

#include <gtest/gtest.h>

TEST(Gc, Algorithm_Interning)
{
    namespace a = gc::alg;
    auto s = a::AlgorithmStorage{};
    auto t1 = s(a::Type{ .name = "int" });
    auto t2 = s(a::Type{ .name = "int" });
    EXPECT_EQ(t1, t2);
    auto t3 = s(a::Type{ .name = "double" });
    EXPECT_NE(t1, t3);
}
