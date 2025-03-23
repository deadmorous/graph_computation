/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "common/func_ref.hpp"

#include <gtest/gtest.h>

#include <numeric>


// -----------------------

namespace {

auto add(int a, int b)
    -> int
{ return a+b; }

auto sum_vec(const std::vector<int>* vec)
    -> int
{ return std::accumulate(vec->begin(), vec->end(), 0); }

struct ClassWithAddMethod
{
    auto add(int a, int b) -> int
    { return a+b; }
};

struct AddFunctor
{
    auto operator()(int a, int b) -> int
    { return a+b; }
};

} // anonymous namespace

// -----------------------


TEST(Common_FuncRef, FreeFunc)
{
    using namespace common;

    auto iref = FuncRef{ Const<add> };
    EXPECT_EQ(iref(3, 4), 7);

    auto iref_i = FuncRef{ Indirect, &add };
    EXPECT_EQ(iref_i(3, 4), 7);
}

TEST(Common_FuncRef, FreeFuncPtr)
{
    using namespace common;

    auto iref = FuncRef{ Indirect, add };
    EXPECT_EQ(iref(3, 4), 7);
}

TEST(Common_FuncRef, FreeFuncWitihContext)
{
    using namespace common;

    auto v = std::vector{ 1, 2, 3 };
    auto iref = FuncRef{ Const<sum_vec>, &v };

    EXPECT_EQ(iref(), 6);
}

TEST(Common_FuncRef, ClassMethod)
{
    using namespace common;

    auto summator = ClassWithAddMethod{};
    auto iref = FuncRef{ Const<&ClassWithAddMethod::add>, &summator };

    EXPECT_EQ(iref(12, 21), 33);
}

TEST(Common_FuncRef, Functor)
{
    using namespace common;

    auto summator = AddFunctor{};
    auto iref = FuncRef{ &summator };
    EXPECT_EQ(iref(3, 4), 7);
}

TEST(Common_FuncRef, CapturelessDecayedLambda)
{
    using namespace common;

    auto decayed_lambda =
        +[](int a, int b) { return a + b; };

    auto iref = FuncRef{ Indirect, decayed_lambda };
    EXPECT_EQ(iref(3, 4), 7);
}

TEST(Common_FuncRef, CapturelessLambda)
{
    using namespace common;

    auto lambda = [](int a, int b) { return a + b; };

    auto iref = FuncRef{ &lambda };
    EXPECT_EQ(iref(3, 4), 7);
}

TEST(Common_FuncRef, LambdaWithCapture)
{
    using namespace common;

    auto five = 5;
    auto lambda = [five](int a) { return a + five; };

    auto iref = FuncRef{ &lambda };
    EXPECT_EQ(iref(3), 8);
}

TEST(Common_FuncRef, Empty)
{
    using namespace common;

    auto iref = FuncRef{ Const<add> };
    EXPECT_TRUE(iref);
    EXPECT_FALSE(iref.empty());

    FuncRef<int(int, int)> empty_iref;
    EXPECT_FALSE(empty_iref);
    EXPECT_TRUE(empty_iref.empty());
}
