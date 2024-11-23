#include "agc_app/linspace.hpp"

#include "gc/activation_node.hpp"

#include "gc/expect_n_node_args.hpp"
#include "gc/node_port_names.hpp"
#include "gc/value.hpp"

#include <cassert>


using namespace gc::literals;
using namespace std::string_view_literals;

namespace agc_app {

class LinSpace final :
    public gc::ActivationNode
{
public:
    auto input_names() const
        -> common::ConstNameSpan override
    { return gc::node_input_names<LinSpace>( "first"sv, "last"sv, "count"sv ); }

    auto output_names() const
        -> common::ConstNameSpan override
    { return gc::node_output_names<LinSpace>( "sequence"sv ); }

    auto default_inputs(gc::InputValues result) const
        -> void override
    {
        assert(result.size() == 3_gc_ic);
        result[0_gc_i] = 0.;
        result[1_gc_i] = 1.;
        result[2_gc_i] = 11;
    }

    auto activation_algorithms(std::span<gc::PortActivationAlgorithm> result,
                               gc::AlgorithmStorage& alg_storage) const
        -> void override
    {
        assert(result.size() == 1);
        auto& result0 = result[0];
        // result0.required_inputs = gc::Ports{0u, 1u, 2u};
    }
};

auto make_linspace(gc::ConstValueSpan args)
    -> std::shared_ptr<gc::ActivationNode>
{
    gc::expect_no_node_args("LinSpace", args);
    return std::make_shared<LinSpace>();
}

} // namespace agc_app
