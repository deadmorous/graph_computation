#include "agc_app/nodes/counter.hpp"

#include "gc/algorithm.hpp"
#include "gc/activation_node.hpp"

#include "gc/expect_n_node_args.hpp"
#include "gc/node_port_names.hpp"
#include "gc/value.hpp"


using namespace gc::literals;
using namespace std::string_view_literals;

namespace agc_app {
namespace {

class Counter final :
    public gc::ActivationNode
{
public:
    auto input_names() const
        -> gc::InputNames override
    {
        return gc::node_input_names<Counter>("next"sv, "reset"sv);
    }

    auto output_names() const
        -> gc::OutputNames override
    { return gc::node_output_names<Counter>("count"sv); }

    auto default_inputs(gc::InputValues result) const
        -> void override
    {
        assert(result.size() == 2_gc_ic);
        result[0_gc_i] = 0;
        result[1_gc_i] = 0;
    }

    auto activation_algorithms(gc::alg::AlgorithmStorage& s) const
        -> gc::NodeActivationAlgorithms override
    {
        namespace a = gc::alg;

        auto result = gc::NodeActivationAlgorithms{};

        // Declare types and symbols

        auto lib =
            s(a::Lib{ .name = "agc_app" });

        auto counter_alg_header =
            s(a::HeaderFile{
                .name = "agc_app/alg/counter.hpp",
                .lib = lib });

        auto uint64_t_header =
            s(a::HeaderFile{
                .name = "cstdint",
                .system = true });

        auto uint64_t_type =
            s(a::Type{
                .name = "uint64_t",
                .header_file = uint64_t_header });

        auto reset_counter_func =
            s(a::Symbol{
                .name = "gc_app::reset_counter",
                .header_file = counter_alg_header });

        auto next_counter_func =
            s(a::Symbol{
                .name = "gc_app::next_counter",
                .header_file = counter_alg_header });

        // Define context variables

        auto count =
            s(a::Var{ uint64_t_type });

        // Define activation algorithms

        auto counter_func_args =
            s(a::Vars{count});

        auto activate_next_statement =
            s(a::Statement{ s(a::FuncInvocation{
                .func = next_counter_func,
                .args = counter_func_args }) });

        result.algorithms.emplace_back(gc::PortActivationAlgorithm{
            .activate = activate_next_statement,
        });

        auto activate_reset_statement =
            s(a::Statement{ s(a::FuncInvocation{
                .func = reset_counter_func,
                .args = counter_func_args }) });

        result.algorithms.emplace_back(gc::PortActivationAlgorithm{
            .activate = activate_reset_statement,
        });

        // A context is required, set `result.context`.
        result.context = counter_func_args;

        return result;
    }
};

} // anonymous namespace


auto make_counter(gc::ConstValueSpan args)
    -> std::shared_ptr<gc::ActivationNode>
{
    gc::expect_no_node_args("Counter", args);
    return std::make_shared<Counter>();
}

} // namespace agc_app
