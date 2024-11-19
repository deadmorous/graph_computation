#include "common/strong.hpp"

#include <gtest/gtest.h>

namespace {

GCLIB_STRONG_TYPE(IntCountDefaultedTo33,
                  int,
                  33,
                  common::StrongCountFeatures);

GCLIB_STRONG_TYPE(Count, ptrdiff_t, 0, common::StrongCountFeatures);
GCLIB_STRONG_TYPE(Index, ptrdiff_t, 0, common::StrongIndexFeatures<Count>);

template <typename Lhs, typename Rhs>
concept CanAdd = requires(Lhs lhs, Rhs rhs)
{ lhs + rhs; };

template <typename Lhs, typename Rhs>
concept CanSub = requires(Lhs lhs, Rhs rhs)
{ lhs - rhs; };

} // anonymous namespace

TEST(Common_Strong, Basic)
{
    {
        using X = IntCountDefaultedTo33;
        X x;
        EXPECT_EQ(x.v, 33);
        static_assert(X::arithmetic);
        static_assert(X::is_count);
        static_assert(!X::is_index);

        X x1 = common::Zero;
        EXPECT_EQ(x1.v, 0);

        X x2{1};
        EXPECT_EQ(x2.v, 1);

        auto sum = x + x2;
        static_assert(std::same_as<decltype(sum), X>);
        EXPECT_EQ(sum.v, 34);

        auto diff = x - x2;
        static_assert(std::same_as<decltype(sum), X>);
        EXPECT_EQ(diff.v, 32);
    }

    {
        auto i0 = Index{};
        auto i1 = Index{1};
        auto i2 = Index{5};
        auto c0 = Count{3};
        auto c1 = Count{4};

        EXPECT_EQ(i0.v, 0);
        EXPECT_EQ(i1.v, 1);
        EXPECT_EQ(i2.v, 5);
        EXPECT_EQ(c0.v, 3);
        EXPECT_EQ(c1.v, 4);

        // Won't compile
        // auto index_plus_index = i0 + i1;
        static_assert(!CanAdd<Index, Index>);

        static_assert(CanSub<Index, Index>);
        auto index_minus_index = i2 - i1;
        static_assert(std::same_as<decltype(index_minus_index), Count>);
        EXPECT_EQ(index_minus_index.v, 4);

        static_assert(CanAdd<Index, Count>);
        auto index_plus_count = i2 + c0;
        static_assert(std::same_as<decltype(index_plus_count), Index>);
        EXPECT_EQ(index_plus_count.v, 8);

        static_assert(CanAdd<Count, Index>);
        auto count_plus_index = c0 + i2;
        static_assert(std::same_as<decltype(count_plus_index), Index>);
        EXPECT_EQ(count_plus_index.v, 8);

        auto index_minus_count = i2 - c0;
        static_assert(std::same_as<decltype(index_minus_count), Index>);
        EXPECT_EQ(index_minus_count.v, 2);
    }
}
