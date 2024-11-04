#include "common/binomial.hpp"

#include <gtest/gtest.h>

TEST(Common_Binomial, Basic)
{
    using T = uint64_t;

    auto check = [](T n, T k, T expected_result)
    { EXPECT_EQ(common::binomial(common::Type<T>, n, k), expected_result); };

    check(0, 0, 1);

    check(1, 0, 1);
    check(1, 1, 1);

    check(2, 0, 1);
    check(2, 1, 2);
    check(2, 2, 1);

    check(3, 0, 1);
    check(3, 1, 3);
    check(3, 2, 3);
    check(3, 3, 1);

    check(4, 0, 1);
    check(4, 1, 4);
    check(4, 2, 6);
    check(4, 3, 4);
    check(4, 4, 1);

    check(5, 0, 1);
    check(5, 1, 5);
    check(5, 2, 10);
    check(5, 3, 10);
    check(5, 4, 5);
    check(5, 5, 1);
}
