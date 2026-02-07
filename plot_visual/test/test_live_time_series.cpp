/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "plot_visual/live_time_series.hpp"

#include <common/index_range.hpp>

#include <gtest/gtest.h>

#include <numeric>


namespace {

using dvec = std::vector<double>;
using drange = plot::CoordinateRange<double>;

auto test_vec(size_t index, size_t size) -> dvec
{
    auto result = dvec(size);
    std::iota(result.begin(), result.end(), index*size);
    return result;
}

} // anonymous namespace

TEST(PlotVisual, LiveTimeSeries_Basic)
{
    constexpr size_t capacity = 5;
    constexpr size_t values_per_frame = 3;

    auto ts = plot::LiveTimeSeries{};
    ts.set_frame_capacity(capacity);

    EXPECT_EQ(ts.frame_capacity(), capacity);

    EXPECT_TRUE(ts.frames().empty());

    for (auto index : common::index_range<size_t>(2*capacity))
    {
        auto v = test_vec(index, values_per_frame);
        ts.add(v);

        auto expected_frame_count = std::min(1+index, capacity);
        auto frames = ts.frames();
        EXPECT_FALSE(frames.empty());
        EXPECT_EQ(frames.size(), expected_frame_count);

        size_t first_frame_ordinal =
            index < capacity ? 0 : index - capacity + 1;

        auto expected_total_range = plot::combine_coordinate_range(
            frames.front().value_range, frames.back().value_range);
        EXPECT_EQ(ts.value_range(), expected_total_range);

        EXPECT_EQ(frames.front().ordinal, first_frame_ordinal);
        EXPECT_EQ(frames.back().ordinal, index);

        for (size_t ordinal = first_frame_ordinal; const auto& frame : frames)
        {
            EXPECT_EQ(frame.ordinal, ordinal);
            auto expected_values = test_vec(ordinal, values_per_frame);
            auto expected_value_range = drange{
                .begin = expected_values.front(),
                .end = expected_values.back() };
            EXPECT_PRED2(std::ranges::equal, frame.values, expected_values);
            EXPECT_EQ(frame.value_range, expected_value_range);
            ++ordinal;
        }
    }
}

TEST(PlotVisual, LiveTimeSeries_FrameByIndex)
{
    constexpr size_t capacity = 5;
    constexpr size_t values_per_frame = 3;

    auto ts = plot::LiveTimeSeries{};
    ts.set_frame_capacity(capacity);

    using Frame = plot::LiveTimeSeries::Frame;
    auto check_frame = [](const Frame& frame, size_t ordinal)
    {
        EXPECT_EQ(frame.ordinal, ordinal);
        auto expected_values = test_vec(ordinal, values_per_frame);
        EXPECT_PRED2(std::ranges::equal, frame.values, expected_values);
    };

    for (size_t index=0; index<3; ++index)
        ts.add(test_vec(index, values_per_frame));

    {
        auto frames = ts.frames();
        check_frame(frames[0], 0);
        check_frame(frames[1], 1);
        check_frame(frames[2], 2);
    }

    for (size_t index=3; index<7; ++index)
        ts.add(test_vec(index, values_per_frame));

    {
        auto frames = ts.frames();
        check_frame(frames[0], 2);
        check_frame(frames[1], 3);
        check_frame(frames[2], 4);
        check_frame(frames[3], 5);
        check_frame(frames[4], 6);
    }
}

TEST(PlotVisual, LiveTimeSeries_Checkpoints)
{
    constexpr size_t capacity = 3;
    constexpr size_t values_per_frame_1 = 3;
    constexpr size_t values_per_frame_2 = 4;

    size_t index = 0;

    auto ts = plot::LiveTimeSeries{};
    ts.set_frame_capacity(capacity);

    auto add_n_frames = [&](size_t n, size_t values_per_frame)
    {
        for (size_t _=0; _<n; ++_)
            ts.add(test_vec(index++, values_per_frame));
    };

    auto c1 = plot::LiveTimeSeries::Checkpoint{};
    auto c2 = plot::LiveTimeSeries::Checkpoint{};

    ts.register_checkpoint(c1);
    ts.register_checkpoint(c2);

    EXPECT_EQ(c1.sync().frames_added, std::nullopt);
    add_n_frames(1, values_per_frame_1);
    EXPECT_EQ(c1.sync().frames_added, std::nullopt);

    add_n_frames(1, values_per_frame_1);
    EXPECT_EQ(c1.sync().frames_added, std::optional<size_t>{1});
    EXPECT_EQ(c1.sync().frames_added, std::optional<size_t>{0});

    add_n_frames(2, values_per_frame_1);
    EXPECT_EQ(c1.sync().frames_added, std::optional<size_t>{2});
    EXPECT_EQ(c1.sync().frames_added, std::optional<size_t>{0});

    EXPECT_EQ(c2.sync().frames_added, std::nullopt);
    add_n_frames(2, values_per_frame_1);
    EXPECT_EQ(c2.sync().frames_added, std::optional<size_t>{2});
    EXPECT_EQ(c2.sync().frames_added, std::optional<size_t>{0});
    add_n_frames(1, values_per_frame_1);
    EXPECT_EQ(c1.sync().frames_added, std::optional<size_t>{3});
    EXPECT_EQ(c1.sync().frames_added, std::optional<size_t>{0});

    add_n_frames(4, values_per_frame_1);
    EXPECT_EQ(c1.sync().frames_added, std::nullopt);

    add_n_frames(1, values_per_frame_1);
    add_n_frames(1, values_per_frame_2);
    EXPECT_EQ(c1.sync().frames_added, std::nullopt);

    add_n_frames(2, values_per_frame_2);
    EXPECT_EQ(c1.sync().frames_added, std::optional<size_t>{2});
    EXPECT_EQ(c1.sync().frames_added, std::optional<size_t>{0});
}
