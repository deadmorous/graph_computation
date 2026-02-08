/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024-2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#include "gc_types/multi_index.hpp"

// #include "common/format.hpp"

#include <gtest/gtest.h>

#include <array>


using namespace gc_types;

TEST(GcTypes, MultiIndex)
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
    check_inc(index2, 3, false, 3, 0);

    auto index3 = std::array{0, 0, 0};
    check_inc(index3, 2, true, 0, 0, 1);
    check_inc(index3, 2, true, 0, 1, 0);
    check_inc(index3, 2, true, 0, 1, 1);
    check_inc(index3, 2, true, 1, 0, 0);
    check_inc(index3, 2, true, 1, 0, 1);
    check_inc(index3, 2, true, 1, 1, 0);
    check_inc(index3, 2, true, 1, 1, 1);
    check_inc(index3, 2, false, 2, 0, 0);
}

TEST(GcTypes, MultiIndexMono)
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
    check_inc(index2, 3, false, 3, 3);

    auto index3 = std::array{0, 0, 0};
    check_inc(index3, 2, true, 0, 0, 1);
    check_inc(index3, 2, true, 0, 1, 1);
    check_inc(index3, 2, true, 1, 1, 1);
    check_inc(index3, 2, false, 2, 2, 2);
}

TEST(GcTypes, MultiIndexMonoSubranges)
{
    using MI = std::vector<uint32_t>;

    auto subrange_length = [](const MI& srb0, const MI& srb1, uint32_t n)
    {
        auto result = uint64_t{};
        for (auto mi=srb0;
             mi != srb1   &&   result < 100'000'000;
             inc_multi_index_mono(mi, n))
        {
            ++result;
        }
        return result;
    };

    auto check_subranges = [&](uint32_t n, uint32_t s, uint32_t p)
    {
        auto range_length = gc_types::multi_index_mono_range_length(n, s);

        std::vector<std::vector<uint32_t>> srb;
        for (uint32_t k=0; k<=p; ++k)
            srb.push_back(
                gc_types::multi_index_mono_subrange_boundary(
                    common::Type<uint32_t>, s, n, k, p));

        auto subrange_length_sum = uint64_t{};
        auto sr_len_min = uint64_t{};
        auto sr_len_max = uint64_t{};

        // std::cout
        //     << "Range n=" << n << ", s=" << s
        //     << ", length=" << range_length << std::endl;
        for (uint32_t k=0; k<p; ++k)
        {
            auto sr_len = subrange_length(srb[k], srb[k+1], n);
            if (k == 0)
                sr_len_min = sr_len_max = sr_len;
            else
            {
                sr_len_min = std::min(sr_len_min, sr_len);
                sr_len_max = std::max(sr_len_max, sr_len);
            }
            // std::cout
            //     << "subrange " << k << "/" << p << ": "
            //     << common::format_seq(srb[k]) << " - "
            //     << common::format_seq(srb[k+1])
            //     << "; length = " << sr_len
            //     << std::endl;
            subrange_length_sum += sr_len;
        }
        EXPECT_EQ(subrange_length_sum, range_length);
        EXPECT_LE(sr_len_max-sr_len_min, 1);
    };

    for (uint32_t p=1; p<10; ++p)
        for (uint32_t s=1; s<10; ++s)
            for (uint32_t n=1; n<10; ++n)
                check_subranges(n, s, p);
}
