/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "common/expr_calculator.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <numbers>


using namespace std::literals;

TEST(Common_ExprCalculator, Basic)
{
    auto calc = [](std::string_view f_x, double x)
    {
        auto vars = common::ExprCalculator::VariableMap{
            {"x"sv, x},
            {"pi"sv, std::numbers::pi},
        };
        return common::ExprCalculator{f_x}(vars);
    };

    EXPECT_EQ(calc("x+1", 3), 4.);
    EXPECT_EQ(calc("x % 7"sv, 13.3), 6);

    static constexpr auto tol = 1e-10;
    auto within_tol = [](double actual, double expected) {
        return std::abs(actual - expected) <= tol;
    };

    EXPECT_PRED2(within_tol, calc("sin(pi)"sv, 123), 0);
    EXPECT_PRED2(within_tol, calc("sin(x)"sv, 1e-5), 1e-5);
    EXPECT_PRED2(
        within_tol,
        calc("127*sin(1.25*3.14159*x/127/8)"sv, 10),
        127*sin(1.25*3.14159*10./127/8));
    EXPECT_THROW(calc("foo + 1"sv, 123), std::runtime_error);
}
