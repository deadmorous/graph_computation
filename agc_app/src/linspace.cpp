#include "agc_app/linspace.hpp"

#include "agc/node.hpp"

#include "gc/expect_n_node_args.hpp"
#include "gc/node_port_names.hpp"
#include "gc/value.hpp"

#include <cassert>


using namespace std::string_view_literals;

namespace agc_app {

class LinSpace final :
    public agc::Node
{
public:
    auto input_names() const
        -> common::ConstNameSpan override
    { return gc::node_input_names<LinSpace>( "first"sv, "last"sv, "count"sv ); }

    auto output_names() const
        -> common::ConstNameSpan override
    { return gc::node_output_names<LinSpace>( "sequence"sv ); }

    auto default_inputs(gc::ValueSpan result) const
        -> void override
    {
        assert(result.size() == 3);
        result[0] = 0.;
        result[1] = 1.;
        result[2] = 11;
    }

    auto activation_algorithms(std::span<agc::PortActivationAlgorithm> result,
                               agc::AlgorithmStorage& alg_storage) const
        -> void override
    {
        assert(result.size() == 1);
        auto& result0 = result[0];
        result0.required_inputs = agc::Ports{0u, 1u, 2u};
    }
};

auto make_linspace(gc::ConstValueSpan args)
    -> std::shared_ptr<agc::Node>
{
    gc::expect_no_node_args("LinSpace", args);
    return std::make_shared<LinSpace>();
}

} // namespace agc_app