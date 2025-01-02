#include "agc_app/nodes/func_iterator.hpp"

#include "gc/algorithm.hpp"
#include "gc/activation_node.hpp"

#include "gc/expect_n_node_args.hpp"
#include "gc/node_port_names.hpp"
#include "gc/value.hpp"


using namespace gc::literals;
using namespace std::string_view_literals;

namespace agc_app {
namespace {

class FuncIterator final :
    public gc::ActivationNode
{
public:
    auto input_names() const
        -> gc::InputNames override
    {
        return gc::node_input_names<FuncIterator>(
            "init"sv, "value"sv, "next"sv);
    }

    auto output_names() const
        -> gc::OutputNames override
    { return gc::node_output_names<FuncIterator>("arg"sv, "value"sv); }

    auto default_inputs(gc::InputValues result) const
        -> void override
    {
        assert(result.size() == 2_gc_ic);
        result[0_gc_i] = std::array<double, 2>{0, 0};
        result[1_gc_i] = std::array<double, 2>{1, 0};
        result[2_gc_i] = 0;
    }

    auto activation_algorithms(gc::alg::AlgorithmStorage& s) const
        -> gc::NodeActivationAlgorithms override
    {
        namespace a = gc::alg;

        auto result = gc::NodeActivationAlgorithms{};

        // Define context variables

        auto value =
            s(a::Var{ a::id::TypeFromBinding{} });

        // Bind input

        auto initial_value =
            s(a::Var{ a::id::TypeFromBinding{} });

        result.input_bindings = {
            s(a::InputBinding{ .port = 0_gc_i, .var = initial_value }),
            s(a::InputBinding{ .port = 1_gc_i, .var = value })
        };

        // Define activation algorithm

        auto activate_init_statement =
            s(a::Statement{ s(a::Assign{
                .dst = value,
                .src = initial_value }) });

        result.algorithms.emplace_back(gc::PortActivationAlgorithm{
            .activate = activate_init_statement,
        });

        auto activate_value_statement =
            s(a::Statement{ s(a::OutputActivation{
                .port = 1_gc_o,
                .var = value }) });

        result.algorithms.emplace_back(gc::PortActivationAlgorithm{
            .activate = activate_value_statement,
        });

        auto activate_next_statement =
            s(a::Statement{ s(a::OutputActivation{
                .port = 0_gc_o,
                .var = value }) });

        result.algorithms.emplace_back(gc::PortActivationAlgorithm{
            .activate = activate_next_statement,
        });

        // A context is required, set `result.context`.
        result.context = s(a::Vars{value});

        return result;
    }
};

} // anonymous namespace


auto make_func_iterator(gc::ConstValueSpan args)
    -> std::shared_ptr<gc::ActivationNode>
{
    gc::expect_no_node_args("FuncIterator", args);
    return std::make_shared<FuncIterator>();
}

} // namespace agc_app
