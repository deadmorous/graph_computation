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

struct ScalarStats final
{
    int64_t sum{};
    int64_t count{};
};

auto plateau_avg_size(const gc_app::I8Image& img, I8Range range)
    -> std::vector<double>
{
    int length = range.length();
    if (length == 0)
        return {};

    int first = range.first();
    int last = first + length - 1;

    if (img.data.empty())
        return std::vector<double>(length, 0.);

    std::vector<ScalarStats> stats(length);

    const auto* pix = img.data.data();
    auto compute_line = [&](size_t i0, size_t i1, size_t stride)
    {
        auto v = pix[i0];
        auto v0 = pix[i0];
        int64_t s = 1;
        auto constant = true;
        for (size_t i=i0; i<i1; i+=stride)
        {
            auto v0 = pix[i];
            if (v0 == v)
                ++s;
            else
            {
                if (v >= first && v <= last)
                {
                    auto& st = stats[v-first];
                    st.sum += s;
                    ++st.count;
                    constant = false;
                }
                v = v0;
                s = 1;
            }
        }
        if (v >= first && v <= last)
        {
            auto& st = stats[v-first];
            st.sum += s;
            if (constant || v != v0) // Because image is on a tor
                ++st.count;
        }
    };

    auto w = img.size.width;
    auto h = img.size.height;

    for (size_t row=0, i0=0; row<h; ++row, i0+=w)
        compute_line(i0, i0+w, 1);

    auto h_w = h*w;
    for (size_t col=0; col<h; ++col)
        compute_line(col, col+h_w, w);

    auto result = stats |
        std::views::transform(
            [](const ScalarStats& x) {
                return x.count == 0 ? 0. : static_cast<double>(x.sum) / x.count;
            });

    return std::vector<double>(result.begin(), result.end());
}

} // anonymous namespace


auto image_metrics(const gc_app::I8Image& img,
                   I8Range state_range,
                   ImageMetricSet metric_types)
    -> ImageMetrics
{
    auto result = ImageMetrics{};
    if (metric_types.contains(ImageMetric::StateHistogram))
        result.histogram = histogram(img, state_range);
    if (metric_types.contains(ImageMetric::EdgeHistogram))
        result.edge_histogram = edge_histogram(img, state_range);
    if (metric_types.contains(ImageMetric::PlateauAvgSize))
        result.plateau_avg_size = plateau_avg_size(img, state_range);
    return result;
}

} // namespace sieve
