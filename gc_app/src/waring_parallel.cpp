#include "gc_app/waring_parallel.hpp"

#include "gc_app/multi_index.hpp"

#include "gc_app/types.hpp"

#include "gc/expect_n_node_args.hpp"
#include "gc/node.hpp"
#include "gc/node_port_names.hpp"

#include "common/binomial.hpp"
#include "common/func_ref.hpp"
#include "common/grouped.hpp"
#include "common/pow2.hpp"

#include <atomic>
#include <cmath>
#include <numeric>
#include <thread>

using namespace std::string_view_literals;

namespace gc_app {
namespace {

auto waring_parallel(Uint limit,
                     Uint s,
                     Uint k,
                     const std::stop_token& stoken,
                     const gc::NodeProgress& progress,
                     Uint thread_count)
    -> std::pair<UintVec, bool>
{
    assert(s > 0);
    assert(k > 1);

    auto local_results = common::Grouped<Uint>{};
    local_results.values.reserve(limit*thread_count);
    for (Uint p=0; p<thread_count; ++p)
    {
        local_results.values.resize(limit*(p+1), 0);
        next_group(local_results);
    }

    auto tlim = static_cast<Uint>(pow(limit, 1./k)) + 1;
    auto n = UintVec(tlim);
    std::iota(n.begin(), n.end(), 0);

    // Compute k-th degrees
    for (auto& nk : n)
    {
        auto n1 = nk;
        for (Uint d=1; d<k; ++d)
            nk *= n1;
    }

    // Partition multi-index range
    auto mi_ranges = common::Grouped<Uint>{};
    mi_ranges.values.reserve(s * (thread_count+1));
    for (Uint p=0; p<=thread_count; ++p)
    {
        mi_ranges.values.resize(s * (p+1), {});
        next_group(mi_ranges);
        multi_index_mono_subrange_boundary(
            group(mi_ranges, p), tlim, p, thread_count);
    }

    // Compute number of iterations; initialize progress-specific variables
    auto iter_count = multi_index_mono_range_length(tlim, s);
    auto progress_factor = 1. / iter_count;
    auto iter = std::atomic<uint64_t>{0};
    auto iter_granularity =
        std::max(2ul, common::ceil2(iter_count / (100*thread_count))) - 1;

    auto threads_succeeded = Uint{};

    auto equal = [](const auto& a, const auto& b)
        -> bool
    { return std::equal(a.begin(), a.end(), b.begin()); };

    std::mutex mut;
    auto last_progress_reported = double{0};
    auto maybe_report_progress = [&]
    {
        if (!progress)
            return;

        double progress_value;
        {
            auto lock = std::lock_guard {mut};
            progress_value = progress_factor * iter;
            if (progress_value - last_progress_reported < 0.01)
                return;
            last_progress_reported = progress_value;
        }
        progress(progress_value);
    };

    auto local_iter_count_lb = iter_count / thread_count;

    // Define thread function
    auto thread_func = [&](Uint p)
    {
        // Compute result within p-th multi-index range
        auto mi0 = group(mi_ranges, p);
        auto mi1 = group(mi_ranges, p+1);

        auto mi = std::vector<Uint>{ mi0.begin(), mi0.end() };

        auto local_iter = uint64_t{};
        auto local_result = group(local_results, p);

        auto do_iter = [&]() noexcept
        {
            Uint sum = 0;
            for (auto i : mi)
                sum += n[i];
            if (sum < limit)
                ++local_result[sum];
            inc_multi_index_mono(mi, tlim);
        };

        // Do most of the iterations without a complex check `equal(mi, mi1)`
        for (; local_iter < local_iter_count_lb; ++local_iter)
        {
            do_iter();

            if (stoken.stop_requested()) [[unlikely]]
                return;

            if ((local_iter & iter_granularity) != 0) [[likely]]
                continue;

            std::atomic_fetch_add(&iter, iter_granularity);
            maybe_report_progress();
        }

        // Do one more iteration if necessary
        if (!equal(mi, mi1))
        {
            do_iter();
            assert(equal(mi, mi1));
        }

        ++threads_succeeded;
    };

    // Start threads
    if (progress)
        progress(0);

    auto threads = std::vector<std::jthread>{};
    threads.reserve(thread_count);
    for (Uint p=0; p<thread_count; ++p)
        threads.emplace_back(thread_func, p);

    // Wait till all threads finish
    for (Uint p=0; p<thread_count; ++p)
        threads[p].join();

    // Sum results within each multi-index range
    auto result = UintVec(limit, 0);
    for (auto p=0; p<thread_count; ++p)
    {
        auto local_result = group(local_results, p);
        for (Uint i=0; i<limit; ++i)
            result[i] += local_result[i];
    }

    return { std::move(result),  threads_succeeded == thread_count };
}

} // anonymous namespace

class WaringParallel final :
    public gc::Node
{
public:
    WaringParallel(Uint thread_count)
        : thread_count_{ thread_count }
    {}

    auto input_names() const
        -> common::ConstNameSpan override
    { return gc::node_input_names<WaringParallel>( "count"sv, "s"sv, "k"sv ); }

    auto output_names() const
        -> common::ConstNameSpan override
    { return gc::node_output_names<WaringParallel>( "sequence"sv ); }

    auto default_inputs(gc::ValueSpan result) const
        -> void override
    {
        assert(result.size() == 3);
        result[0] = uint_val(1000);
        result[1] = uint_val(2);
        result[2] = uint_val(2);
    }

    auto compute_outputs(
            gc::ValueSpan result,
            gc::ConstValueSpan inputs,
            const std::stop_token& stoken,
            const gc::NodeProgress& progress) const
        -> bool override
    {
        assert(inputs.size() == 3);
        assert(result.size() == 1);
        auto count = uint_val(inputs[0]);
        auto s = uint_val(inputs[1]);
        auto k = uint_val(inputs[2]);
        auto [seq, computed] =
            waring_parallel(count, s, k, stoken, progress, thread_count_);
        result[0] = uint_vec_val(std::move(seq));
        return computed;
    }

private:
    Uint thread_count_;
};

auto make_waring_parallel(gc::ConstValueSpan args)
    -> std::shared_ptr<gc::Node>
{
    gc::expect_n_node_args("WaringParallel", args, 1);
    auto thread_count = args[0].convert_to<Uint>();
    return std::make_shared<WaringParallel>(thread_count);
}

} // namespace gc_app
