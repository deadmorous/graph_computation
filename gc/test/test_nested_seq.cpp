/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "gc/detail/nested_seq.hpp"

#include <gtest/gtest.h>


namespace {

struct Even final
{
    auto operator()(unsigned int x) -> bool
    { return (x & 1) == 0; }
};

struct Ascending final
{
    unsigned int state_{};

    auto operator()(unsigned int x) -> bool
    {
        if (x <= state_)
            return false;
        state_ = x;
        return true;
    }
};

} // anonymous namespace


TEST(Gc, NestedSeq)
{
    using Seq = gc::detail::NestedSequence<unsigned int>;
    using ScopedSeq = gc::detail::ScopedNestedSequence<unsigned int>;
    using gc::detail::SeqType;

    constexpr auto beg_all = gc::detail::BeginSeqMark{ SeqType::AllInOrder };
    constexpr auto beg_every = gc::detail::BeginSeqMark{ SeqType::Every };
    constexpr auto end = gc::detail::EndSeqMark{};

    auto test_seq = [](Seq seq, auto pred, bool expected)
    {
        auto actual = gc::detail::test_sequence(seq, pred);
        EXPECT_EQ(actual, expected);
    };

    auto even = Even{};

    test_seq({ beg_all, end }, even, true);
    test_seq({ beg_every, end }, even, true);
    test_seq({ beg_all, 2u, end }, even, true);
    test_seq({ beg_all, 2u, 4u, 0u, end }, even, true);
    test_seq({ beg_all, 2u, 4u, 7u, end }, even, false);
    test_seq({ beg_all, 2u, 4u, beg_every, 8u, 4u, end, end }, even, true);
    test_seq({ beg_all, 1u, end }, even, false);
    test_seq({ beg_all, 2u, end, beg_all, 4u, 7u, end }, even, false);
    test_seq({ beg_all, 2u, end, beg_all, 4u, 8u, end }, even, true);

    EXPECT_THROW(
        test_seq({ beg_all, 2u }, even, true),
        std::invalid_argument);
    EXPECT_THROW(
        test_seq({ 2u, end }, even, true),
        std::invalid_argument);
    EXPECT_THROW(
        test_seq({}, even, true),
        std::invalid_argument);
    EXPECT_THROW(
        test_seq({ beg_all, 2u, 4u, beg_every, 8u, 4u, end }, even, true),
        std::invalid_argument);


    auto ascending = Ascending{};

    test_seq({ beg_all, 2u, 4u, 0u, end }, ascending, false);
    test_seq({ beg_all, 2u, 4u, 5u, end }, ascending, true);
    test_seq({ beg_every, 3u, 2u, 1u, end }, ascending, true);
    test_seq({ beg_all, 3u, 2u, 1u, end }, ascending, false);
    test_seq({ beg_every,
               beg_all, 2u, 4u, 5u, end,
               beg_all, 3u, 4u, 6u, end,
               end }, ascending, true);
    test_seq({ beg_all,
               beg_all, 2u, 4u, 5u, end,
               beg_all, 3u, 4u, 6u, end,
               end }, ascending, false);
    test_seq({ beg_all,
               1u,
               beg_every,
               beg_all, 2u, 4u, 5u, end,
               beg_all, 3u, 4u, 6u, end,
               end,
               end }, ascending, true);
    test_seq({ beg_all,
               2u,
               beg_every,
               beg_all, 2u, 4u, 5u, end,
               beg_all, 3u, 4u, 6u, end,
               end,
               end }, ascending, false);
}
