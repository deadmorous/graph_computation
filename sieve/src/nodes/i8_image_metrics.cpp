/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#include "sieve/nodes/i8_image_metrics.hpp"

#include "sieve/algorithms/image_metrics.hpp"

#include "gc/expect_n_node_args.hpp"
#include "gc/computation_node.hpp"
#include "gc/node_port_names.hpp"
#include "gc/value.hpp"

#include "common/func_ref.hpp"

using namespace std::literals;
using namespace gc::literals;

namespace sieve {

class I8ImageMetricsNode final :
    public gc::ComputationNode
{
public:
    auto input_names() const
        -> gc::InputNames override
    {
        return gc::node_input_names<I8ImageMetricsNode>(
            "image"sv, "min_state"sv, "state_count"sv);
    }

    auto output_names() const
        -> gc::OutputNames override
    { return gc::node_output_names<I8ImageMetricsNode>("image_metrics"sv); }

    auto default_inputs(gc::InputValues result) const
        -> void override
    {
        assert(result.size() == 3_gc_ic);
        result[0_gc_i] = gc_app::I8Image{};
        result[1_gc_i] = 0;
        result[2_gc_i] = 2;
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
        const auto& image = inputs[0_gc_i].as<gc_app::I8Image>();
        auto min_state = inputs[1_gc_i].convert_to<int>();
        auto state_count = inputs[2_gc_i].convert_to<int>();
        result[0_gc_o] = image_metrics(image, {min_state, state_count});

        if (progress)
            progress(1);

        return true;
    }
};

auto make_i8_image_metrics(gc::ConstValueSpan args,
                           const gc::ComputationContext&)
    -> std::shared_ptr<gc::ComputationNode>
{
    gc::expect_no_node_args("I8ImageMetricsNode", args);
    return std::make_shared<I8ImageMetricsNode>();
}

} // namespace sieve
