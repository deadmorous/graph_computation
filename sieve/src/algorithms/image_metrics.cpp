/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "sieve/algorithms/image_metrics.hpp"

#include <ranges>

namespace sieve {

namespace {

auto normalize(const std::vector<uint32_t>& v, double factor)
    -> std::vector<double>
{
    auto result = v
        | std::views::transform([factor](uint32_t x) { return x * factor; });

    return std::vector<double>(result.begin(), result.end());
}

auto histogram(const gc_app::I8Image& img, I8Range range) -> std::vector<double>
{
    int length = range.length();
    if (length == 0)
        return {};

    int first = range.first();
    int last = first + length - 1;

    if (img.data.empty())
        return std::vector<double>(length, 0.);

    std::vector<uint32_t> counters(length, 0);
    for (auto v : img.data)
    {
        if (v < first || v > last)
            // Should not really hapen and indicates an invalid image
            // since it has out-of-range pixels
            continue;

        ++counters[v - first];
    }

    return normalize(counters, 1./img.data.size());
}

auto edge_histogram(const gc_app::I8Image& img, I8Range range)
    -> std::vector<double>
{
    size_t length = range.length();
    if (length == 0)
        return {};

    int first = range.first();
    int last = first + length - 1;

    auto bucket_count = length;

    if (img.data.empty())
        return std::vector<double>(bucket_count, 0.);

    std::vector<uint32_t> counters(bucket_count, 0);

    auto width = img.size.width;
    auto height = img.size.height;

    auto process = [&](int v0, int v1)
    {
        if (v0 < first || v0 > last || v1 < first || v1 > last)
            // Should not really hapen and indicates an invalid image
            // since it has out-of-range pixels
            return;
        auto index = std::abs(v1 - v0);
        ++counters[index];
    };

    const auto* prev_row = img.data.data() + (height-1) * width;
    const auto* row = img.data.data();
    for (auto y=0; y<height; ++y, prev_row=row, row+=width)
    {
        for (auto x=0; x+1<width; ++x)
        {
            process(row[x], row[x+1]);
            process(prev_row[x], row[x]);
        }
        process(row[width-1], row[0]);
        process(prev_row[width-1], row[width-1]);
    }

    return normalize(counters, 0.5/img.data.size());
}

} // anonymous namespace


auto image_metrics(const gc_app::I8Image& img, I8Range state_range)
    -> ImageMetrics
{
    return {
        .histogram = histogram(img, state_range),
        .edge_histogram = edge_histogram(img, state_range)
    };
}

} // namespace sieve
