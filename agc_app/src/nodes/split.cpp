#include "agc_app/nodes/split.hpp"

#include "gc/algorithm.hpp"
#include "gc/activation_node.hpp"

#include "gc/expect_n_node_args.hpp"
#include "gc/node_port_names.hpp"
#include "gc/value.hpp"


using namespace gc::literals;
using namespace std::string_view_literals;

namespace agc_app {
namespace {

class Split final :
    public gc::ActivationNode
{
public:
    explicit Split(gc::OutputPortCount output_port_count):
        output_port_names_{ output_port_count }
    {}

    auto input_names() const
        -> gc::InputNames override
    {
        return gc::node_input_names<Split>("in"sv);
    }

    auto output_names() const
        -> gc::OutputNames override
    { return output_port_names_(); }

    auto default_inputs(gc::InputValues result) const
        -> void override
    {
        assert(result.size() == 1_gc_ic);
        result[0_gc_i] = 0;
    }

    auto activation_algorithms(gc::alg::AlgorithmStorage& s) const
        -> gc::NodeActivationAlgorithms override
    {
        namespace a = gc::alg;

        auto result = gc::NodeActivationAlgorithms{};

        // Bind input

        auto input = s(a::Var{ a::id::TypeFromBinding{} });

        result.input_bindings = {
            s(a::InputBinding{ .port = 0_gc_i, .var = input })
        };

        // Define activation algorithm
        auto activation_seq = a::Block{};
        for (auto port : output_port_names_().index_range())
        {
            activation_seq.statements.push_back(
                s(a::Statement{ s(a::OutputActivation{
                    .port = port,
                    .var = input
                }) }));
        }

        auto activate_statement =
            s(a::Statement{ s(activation_seq) });

        result.algorithms.emplace_back(gc::PortActivationAlgorithm{
            .activate = activate_statement
        });

        // No context is required, node is stateless -
        // not setting `result.context`.

        return result;
    }

private:
    gc::DynamicOutputNames output_port_names_;
};

} // anonymous namespace


auto make_split(gc::ConstValueSpan args)
    -> std::shared_ptr<gc::ActivationNode>
{
    gc::expect_n_node_args("Split", args, 1);
    auto port_count = args[0].convert_to<gc::WeakPort>();
    return std::make_shared<Split>(gc::OutputPortCount{port_count});
}

} // namespace agc_app
