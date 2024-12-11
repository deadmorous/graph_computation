#include "common/index_set.hpp"
#include "common/format.hpp"

#include <gtest/gtest.h>

#include <cstdint>

namespace {

GCLIB_STRONG_TYPE(Count, uint8_t, 0, common::StrongCountFeatures);
GCLIB_STRONG_TYPE(Index, uint8_t, 0, common::StrongIndexFeatures<Count>);

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
    EXPECT_EQ(s.size(), 1);
    EXPECT_FALSE(s.empty());
    check_values(s, {2});
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
    EXPECT_EQ(s.size(), 1);
    EXPECT_FALSE(s.empty());
    check_values(s, {2_i});

    auto str =
        common::format_seq(
            S{1_i, 5_i},
            ", ",
            [](std::ostream& s, Index i)
                { s << static_cast<int>(i.v) << "_i"; });
    EXPECT_EQ(str, "1_i, 5_i");
}
