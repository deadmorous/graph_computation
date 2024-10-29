#include "gc_app/multi_index.hpp"

#include "common/format.hpp"

#include <gtest/gtest.h>

#include <array>


using namespace gc_app;

TEST(GcApp, MultiIndex)
{
    auto check_inc = [](auto& index, auto n, bool b, auto... i)
    {
        EXPECT_EQ(b, inc_multi_index(index, n));
        EXPECT_EQ(index, std::array{i...});
    };

    auto index2 = std::array{0, 0};
    check_inc(index2, 3, true, 0, 1);
    check_inc(index2, 3, true, 0, 2);
    check_inc(index2, 3, true, 1, 0);
    check_inc(index2, 3, true, 1, 1);
    check_inc(index2, 3, true, 1, 2);
    check_inc(index2, 3, true, 2, 0);
    check_inc(index2, 3, true, 2, 1);
    check_inc(index2, 3, true, 2, 2);
    check_inc(index2, 3, false, 0, 0);

    auto index3 = std::array{0, 0, 0};
    check_inc(index3, 2, true, 0, 0, 1);
    check_inc(index3, 2, true, 0, 1, 0);
    check_inc(index3, 2, true, 0, 1, 1);
    check_inc(index3, 2, true, 1, 0, 0);
    check_inc(index3, 2, true, 1, 0, 1);
    check_inc(index3, 2, true, 1, 1, 0);
    check_inc(index3, 2, true, 1, 1, 1);
    check_inc(index3, 2, false, 0, 0, 0);
}

TEST(GcApp, MultiIndexMono)
{
    auto check_inc = [](auto& index, auto n, bool b, auto... i)
    {
        EXPECT_EQ(b, inc_multi_index_mono(index, n));
        EXPECT_EQ(index, std::array{i...});
    };

    auto index2 = std::array{0, 0};
    check_inc(index2, 3, true, 0, 1);
    check_inc(index2, 3, true, 0, 2);
    check_inc(index2, 3, true, 1, 1);
    check_inc(index2, 3, true, 1, 2);
    check_inc(index2, 3, true, 2, 2);
    check_inc(index2, 3, false, 2, 2);

    auto index3 = std::array{0, 0, 0};
    check_inc(index3, 2, true, 0, 0, 1);
    check_inc(index3, 2, true, 0, 1, 1);
    check_inc(index3, 2, true, 1, 1, 1);
    check_inc(index3, 2, false, 1, 1, 1);
}
