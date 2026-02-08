/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025-2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "sieve/algorithms/image_metrics.hpp"

#include <gtest/gtest.h>


TEST(Sieve_ImageMetrics, Basic)
{
    auto check_image_metrics = [](
        const gc_types::I8Image& image,
        const sieve::I8Range& range,
        const sieve::ImageMetrics& expected_metrics)
    {
        auto all_types = sieve::ImageMetricSet::full();
        auto metrics = sieve::image_metrics(image, range, all_types);
        EXPECT_EQ(metrics.histogram, expected_metrics.histogram);
        EXPECT_EQ(metrics.edge_histogram, expected_metrics.edge_histogram);
        // TODO: Test plateau_avg_size
    };

    constexpr auto empty_image = gc_types::I8Image{};
    const auto i_4x2_0123_4567 = gc_types::I8Image{
        .size{4, 2},
        .data{0, 1, 2, 3,
              4, 5, 6, 7}};

    const auto i_4x4_checkers_1 = gc_types::I8Image{
        .size{4, 4},
        .data{0, 1, 0, 1,
              1, 0, 1, 0,
              0, 1, 0, 1,
              1, 0, 1, 0}};

    const auto i_4x4_checkers_2 = gc_types::I8Image{
        .size{4, 4},
        .data{0, 0, 1, 1,
              0, 0, 1, 1,
              1, 1, 0, 0,
              1, 1, 0, 0}};


    constexpr auto empty_range = sieve::I8Range{};
    constexpr auto empty_metrics = sieve::ImageMetrics{};

    check_image_metrics(empty_image, empty_range, empty_metrics);

    check_image_metrics(i_4x2_0123_4567, empty_range, empty_metrics);
    check_image_metrics(i_4x4_checkers_1, empty_range, empty_metrics);
    check_image_metrics(i_4x4_checkers_2, empty_range, empty_metrics);

    check_image_metrics(
        i_4x2_0123_4567,
        {1, 2},
        {
            .histogram = std::vector<double>(2, 0.125),
            .edge_histogram = {0, 0.0625}
        });

    check_image_metrics(
        i_4x2_0123_4567,
        {0, 4},
        {
            .histogram = std::vector<double>(4, 0.125),
            .edge_histogram = {0, 0.1875, 0, 0.0625}
        });

    check_image_metrics(
        i_4x2_0123_4567,
        {0, 8},
        {
         .histogram = std::vector<double>(8, 0.125),
         .edge_histogram = {0, 0.375, 0, 0.125, 0.5, 0, 0, 0}
        });

    check_image_metrics(
        i_4x4_checkers_1,
        {0, 2},
        {
         .histogram = {0.5, 0.5},
         .edge_histogram = {0, 1}
        });

    check_image_metrics(
        i_4x4_checkers_2,
        {0, 2},
        {
         .histogram = {0.5, 0.5},
         .edge_histogram = {0.5, 0.5}
        });
}
