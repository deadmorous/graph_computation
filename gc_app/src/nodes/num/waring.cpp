/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024-2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#include "gc_app/nodes/num/waring.hpp"

#include "gc_types/multi_index.hpp"
#include "gc_types/uint_vec.hpp"

#include "gc/expect_n_node_args.hpp"
#include "gc/computation_node.hpp"
#include "gc/node_port_names.hpp"

#include "common/binomial.hpp"
#include "common/func_ref.hpp"
#include "common/pow2.hpp"

#include <cmath>
#include <numeric>

using namespace std::string_view_literals;
using namespace gc::literals;

namespace gc_app::num {

using namespace gc_types;

namespace {

auto waring(Uint limit,
            Uint s,
            Uint k,
            const std::stop_token& stoken,
            const gc::NodeProgress& progress)
    -> std::pair<UintVec, bool>
{
    assert(s > 0);
    assert(k > 1);

    auto result = UintVec(limit, 0);

    auto tlim = static_cast<Uint>(pow(limit, 1./k)) + 1;
    auto n = UintVec(tlim);
    std::iota(n.begin(), n.end(), 0);

    for (auto& nk : n)
    {
        auto n1 = nk;
        for (Uint d=1; d<k; ++d)
            nk *= n1;
    }

    auto index = std::vector<Uint>(s, 0);

    auto iter_count = binomial(common::Type<uint64_t>, tlim+s-1, s);
    auto progress_factor = 1. / iter_count;
    auto iter = uint64_t{0};
    auto iter_granularity =
        std::max(2ul, common::ceil2(iter_count / 100)) - 1;

    do
    {
        Uint sum = 0;
        for (auto i : index)
            sum += n[i];
        if (sum < limit)
            ++result[sum];

        if (stoken.stop_requested()) [[unlikely]]
            return { std::move(result), false };

        if (!progress) [[unlikely]]
            continue;

        ++iter;

        if ((iter & iter_granularity) != 0) [[likely]]
            continue;

        progress(progress_factor*iter);
    }
    while(inc_multi_index_mono(index, tlim));

    return { std::move(result), true };
}

} // anonymous namespace

class Waring final :
    public gc::ComputationNode
{
public:
    auto input_names() const
        -> gc::InputNames override
    { return gc::node_input_names<Waring>( "count"sv, "s"sv, "k"sv ); }

    auto output_names() const
        -> gc::OutputNames override
    { return gc::node_output_names<Waring>( "sequence"sv ); }

    auto default_inputs(gc::InputValues result) const
        -> void override
    {
        assert(result.size() == 3_gc_ic);
        result[0_gc_i] = uint_val(1000);
        result[1_gc_i] = uint_val(2);
        result[2_gc_i] = uint_val(2);
    }

    auto compute_outputs(
            gc::OutputValues result,
            gc::ConstInputValues inputs,
            const std::stop_token& stoken,
            const gc::NodeProgress& progress) const
        -> bool override
    {
        assert(inputs.size() == 3_gc_ic);
        assert(result.size() == 1_gc_oc);
        auto count = uint_val(inputs[0_gc_i]);
        auto s = uint_val(inputs[1_gc_i]);
        auto k = uint_val(inputs[2_gc_i]);
        auto [seq, computed] = waring(count, s, k, stoken, progress);
        result.front() = uint_vec_val(std::move(seq));
        return computed;
    }
};

auto make_waring(gc::ConstValueSpan args, const gc::ComputationContext&)
    -> std::shared_ptr<gc::ComputationNode>
{
    gc::expect_no_node_args("Waring", args);
    return std::make_shared<Waring>();
}

} // namespace gc_app::num
