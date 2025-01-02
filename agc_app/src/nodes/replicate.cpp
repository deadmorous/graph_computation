#include "agc_app/nodes/replicate.hpp"

#include "gc/algorithm.hpp"
#include "gc/activation_node.hpp"

#include "gc/expect_n_node_args.hpp"
#include "gc/node_port_names.hpp"
#include "gc/value.hpp"


using namespace gc::literals;
using namespace std::string_view_literals;

namespace agc_app {
namespace {

class Replicate final :
    public gc::ActivationNode
{
public:
    auto input_names() const
        -> gc::InputNames override
    { return gc::node_input_names<Replicate>("value"sv, "trigger"sv); }

    auto output_names() const
        -> gc::OutputNames override
    { return gc::node_output_names<Replicate>("value"sv); }

    auto default_inputs(gc::InputValues result) const
        -> void override
    {
        assert(result.size() == 2_gc_ic);
        result[0_gc_i] = 1.23;
        result[1_gc_i] = 0;
    }

    auto activation_algorithms(gc::alg::AlgorithmStorage& s) const
        -> gc::NodeActivationAlgorithms override
    {
        namespace a = gc::alg;

        auto result = gc::NodeActivationAlgorithms{};

        // Bind input

        auto value = s(a::Var{ a::id::TypeFromBinding{} });

        result.input_bindings = {
            s(a::InputBinding{ .port = 0_gc_i, .var = value })
        };

        // Define activation algorithm

        auto activate_value_statement =
            s(a::Statement{ s(a::Block{}) });

        result.algorithms.emplace_back(gc::PortActivationAlgorithm{
            .required_inputs = {0_gc_i},
            .activate = activate_value_statement,
        });

        auto activate_trigger_statement =
            s(a::Statement{ s(a::OutputActivation{
                .port = 0_gc_o,
                .var = value
            }) });

        result.algorithms.emplace_back(gc::PortActivationAlgorithm{
            .required_inputs = {0_gc_i},
            .activate = activate_trigger_statement,
        });

        // `value` constitutes node state, no need to specify additional
        // `result.context`.

        return result;
    }
};

} // anonymous namespace


auto make_replicate(gc::ConstValueSpan args)
    -> std::shared_ptr<gc::ActivationNode>
{
    gc::expect_no_node_args("Replicate", args);
    return std::make_shared<Replicate>();
}

} // namespace agc_app
