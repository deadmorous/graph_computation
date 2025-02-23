#include "common/index_set.hpp"
#include "common/format.hpp"

#include <gtest/gtest.h>

#include <cstdint>

namespace {

GCLIB_STRONG_TYPE(Count, uint8_t, common::StrongCountFeatures);
GCLIB_STRONG_TYPE(Index, uint8_t, common::StrongIndexFeatures<Count>);

GCLIB_STRONG_LITERAL_SUFFIX(Index, _i);

} // anonymous namespace


TEST(Common_IndexSet, Weak)
{
    using S = common::IndexSet<uint8_t, 10>;
    using Vec = std::vector<uint8_t>;

    auto s = S::all();

    for (uint8_t i=0; i<100; ++i)
        EXPECT_EQ(s.contains(i), i<10);

    EXPECT_EQ(s.size(), 10);
    EXPECT_FALSE(s.empty());

    auto check_values = [](const S& s, const Vec& expected)
    {
        auto values = Vec( s.begin(), s.end() );
        EXPECT_EQ(values, expected);
    };

    check_values(s, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9});

    s.toggle(3);
    EXPECT_EQ(s.size(), 9);
    EXPECT_FALSE(s.empty());
    check_values(s, {0, 1, 2, 4, 5, 6, 7, 8, 9});


    s = S{};
    EXPECT_EQ(s.size(), 0);
    EXPECT_TRUE(s.empty());
    check_values(s, {});

    s.clear(2);
    s.toggle(7);
    s.set(8);
    EXPECT_EQ(s.size(), 2);
    EXPECT_FALSE(s.empty());
    check_values(s, {7, 8});

    s.set(2);
    s.toggle(7);
    s.clear(8);
    s.set({5, 6});
    EXPECT_EQ(s.size(), 3);
    EXPECT_FALSE(s.empty());
    check_values(s, {2, 5, 6});

    auto s1 = S{ 0, 1, 2, 3, 4 };
    check_values(~s1, {5, 6, 7, 8, 9});
    check_values(s | s1, {0, 1, 2, 3, 4, 5, 6});
    check_values(s & s1, {2});
    check_values(s ^ s1, {0, 1, 3, 4, 5, 6});

    s1 ^= s1;
    ASSERT_TRUE(s1.empty());
    check_values(s1, {});
}



TEST(Common_IndexSet, Strong)
{
    using S = common::IndexSet<Index, 10>;
    using Vec = std::vector<Index>;

    auto s = S::all();

    for (auto i : common::index_range<Index>(Count{100}))
        EXPECT_EQ(s.contains(i), i<Index{10});

    EXPECT_EQ(s.size(), 10);
    EXPECT_FALSE(s.empty());

    auto check_values = [](const S& s, const Vec& expected)
    {
        auto values = Vec( s.begin(), s.end() );
        EXPECT_EQ(values, expected);
    };

    check_values(s, {0_i, 1_i, 2_i, 3_i, 4_i, 5_i, 6_i, 7_i, 8_i, 9_i});

    s.toggle(3_i);
    EXPECT_EQ(s.size(), 9);
    EXPECT_FALSE(s.empty());
    check_values(s, {0_i, 1_i, 2_i, 4_i, 5_i, 6_i, 7_i, 8_i, 9_i});


    s = S{};
    EXPECT_EQ(s.size(), 0);
    EXPECT_TRUE(s.empty());
    check_values(s, {});

    s.clear(2_i);
    s.toggle(7_i);
    s.set(8_i);
    EXPECT_EQ(s.size(), 2);
    EXPECT_FALSE(s.empty());
    check_values(s, {7_i, 8_i});

    s.set(2_i);
    s.toggle(7_i);
    s.clear(8_i);
    s.set({5_i, 6_i});
    EXPECT_EQ(s.size(), 3);
    EXPECT_FALSE(s.empty());
    check_values(s, {2_i, 5_i, 6_i});

    auto s1 = S{ 0_i, 1_i, 2_i, 3_i, 4_i };
    check_values(~s1, {5_i, 6_i, 7_i, 8_i, 9_i});
    check_values(s | s1, {0_i, 1_i, 2_i, 3_i, 4_i, 5_i, 6_i});
    check_values(s & s1, {2_i});
    check_values(s ^ s1, {0_i, 1_i, 3_i, 4_i, 5_i, 6_i});

    s1 ^= s1;
    ASSERT_TRUE(s1.empty());
    check_values(s1, {});

    auto str =
        common::format_seq(
            S{1_i, 5_i},
            ", ",
            [](std::ostream& s, Index i)
                { s << static_cast<int>(i.v) << "_i"; });
    EXPECT_EQ(str, "1_i, 5_i");
}
