#include "common/strong.hpp"
#include "common/index_range.hpp"

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

TEST(Common_Strong, Arithmetic)
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

    static_assert(!CanAdd<Index, Index>);

    static_assert(CanAdd<Index, Count>);
    auto index_plus_count = i2 + c0;
    static_assert(std::same_as<decltype(index_plus_count), Index>);
    EXPECT_EQ(index_plus_count.v, 8);

    static_assert(CanAdd<Count, Index>);
    auto count_plus_index = c0 + i2;
    static_assert(std::same_as<decltype(count_plus_index), Index>);
    EXPECT_EQ(count_plus_index.v, 8);

    static_assert(CanAdd<Count, Count>);
    auto count_plus_count = c0 + c1;
    static_assert(std::same_as<decltype(count_plus_count), Count>);
    EXPECT_EQ(count_plus_count.v, 7);



    static_assert(CanSub<Index, Index>);
    auto index_minus_index = i2 - i1;
    static_assert(std::same_as<decltype(index_minus_index), Count>);
    EXPECT_EQ(index_minus_index.v, 4);

    static_assert(CanSub<Index, Count>);
    auto index_minus_count = i2 - c0;
    static_assert(std::same_as<decltype(index_minus_count), Index>);
    EXPECT_EQ(index_minus_count.v, 2);

    static_assert(!CanSub<Count, Index>);

    static_assert(CanSub<Count, Count>);
    auto count_minus_count = c0 - c1;
    static_assert(std::same_as<decltype(count_minus_count), Count>);
    EXPECT_EQ(count_minus_count.v, -1);

    EXPECT_EQ(i1++, Index{1});
    EXPECT_EQ(i1, Index{2});
    EXPECT_EQ(++i1, Index{3});
    EXPECT_EQ(i1, Index{3});
    EXPECT_EQ(i1++, Index{3});
    EXPECT_EQ(i1, Index{4});

    EXPECT_EQ(i1--, Index{4});
    EXPECT_EQ(i1, Index{3});
    EXPECT_EQ(--i1, Index{2});
    EXPECT_EQ(i1, Index{2});
    EXPECT_EQ(i1--, Index{2});
    EXPECT_EQ(i1, Index{1});

    i1 += c0;
    EXPECT_EQ(i1, Index{4});
    i1 -= c0;
    EXPECT_EQ(i1, Index{1});

    c0 += c1;
    EXPECT_EQ(c0, Count{7});
    c0 -= c1;
    EXPECT_EQ(c0, Count{3});
}

TEST(Common_Strong, Ranges)
{
    std::cout << "-----\n";
    auto i1 = Index{5};
    auto i2 = Index{10};
    auto c = Count{10};

    auto test_range =
        []<typename R, typename I>(
            R range, common::Type_Tag<I>, size_t begin, size_t size)
    {
        auto has_first = false;
        auto iter_count = size_t{};
        I first;
        for (auto i : range)
        {
            static_assert(std::same_as<I, decltype(i)>);
            if (!has_first)
            {
                first = i;
                has_first = true;
            }
            EXPECT_EQ(i, first+typename R::Count(iter_count));
            ++iter_count;
        }
        EXPECT_EQ(typename R::Count(iter_count), range.size());
        EXPECT_EQ(iter_count, size);
    };

    constexpr auto index_tag = common::Type<Index>;
    constexpr auto int_tag = common::Type<int>;
    test_range(common::index_range(i1, i2), index_tag, i1.v, (i2-i1).v);
    test_range(common::index_range(i1, c), index_tag, i1.v, c.v);
    test_range(common::sized_index_range(i1, c), index_tag, i1.v, c.v);
    test_range(common::index_range<Index>(c), index_tag, 0, c.v);
    test_range(common::index_range(5, 10), int_tag, 5, 10-5);
    test_range(common::sized_index_range(5, 10), int_tag, 5, 10);
    test_range(common::index_range(10), int_tag, 0, 10);
}
