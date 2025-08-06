#include "agc_app/nodes/counter.hpp"

#include "agc_app/alg_lib.hpp"

#include "gc/algorithm.hpp"
#include "gc/alg_known_types.hpp"
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

    auto exported_types(gc::ExportedTypes& xt,
                        gc::alg::AlgorithmStorage& s) const
        -> void override
    {
        // Declare exported types
        namespace a = gc::alg;
        xt[counter_type] = a::well_known_type(a::uint64_t_type, s);
    }

    auto activation_algorithms(gc::alg::AlgorithmStorage& s) const
        -> gc::NodeActivationAlgorithms override
    {
        namespace a = gc::alg;

        auto result = gc::NodeActivationAlgorithms{};

        // Declare types and symbols

        const auto xt = ActivationNode::exported_types(s);

        auto lib = alg_lib(s);

        auto counter_alg_header =
            s(a::HeaderFile{
                .name = "agc_rt/nodes/counter.hpp",
                .lib = lib });

        auto c_type = xt.at(counter_type);

        auto reset_counter_func =
            s(a::Symbol{
                .name = "agc_rt::reset_counter",
                .header_file = counter_alg_header });

        auto next_counter_func =
            s(a::Symbol{
                .name = "agc_rt::next_counter",
                .header_file = counter_alg_header });

        // Define context variables

        auto count =
            s(a::Var{ c_type });

        // Define activation algorithms

        auto counter_func_args =
            s(a::Vars{count});

        auto activate_next_statement =
            s(a::Statement{ s(a::Block{
                .statements = {
                    s(a::Statement{ s(a::FuncInvocation{
                        .func = next_counter_func,
                        .args = counter_func_args }) }),
                    s(a::Statement{ s(a::OutputActivation{
                        .port = 0_gc_o,
                        .var = count }) }) }}) });

        result.algorithms.emplace_back(gc::PortActivationAlgorithm{
            .activate = activate_next_statement
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

    GCLIB_DECL_ACTIVATION_NODE_META(Counter);
};

} // anonymous namespace


auto make_counter(gc::ConstValueSpan args)
    -> std::shared_ptr<gc::ActivationNode>
{
    gc::expect_no_node_args(Counter::static_meta().type_name, args);
    return std::make_shared<Counter>();
}

} // namespace agc_app
