#include "agc_app/nodes/threshold.hpp"

#include "gc/algorithm.hpp"
#include "gc/activation_node.hpp"

#include "gc/expect_n_node_args.hpp"
#include "gc/node_port_names.hpp"
#include "gc/value.hpp"


using namespace gc::literals;
using namespace std::string_view_literals;

namespace agc_app {
namespace {

class Threshold final :
    public gc::ActivationNode
{
public:
    auto input_names() const
        -> gc::InputNames override
    { return gc::node_input_names<Threshold>("threshold"sv, "value"sv); }

    auto output_names() const
        -> gc::OutputNames override
    { return gc::node_output_names<Threshold>("pass"sv, "fail"sv); }

    auto default_inputs(gc::InputValues result) const
        -> void override
    {
        assert(result.size() == 2_gc_ic);
        result[0_gc_i] = 1.23;
        result[1_gc_i] = 4.5;
    }

    auto activation_algorithms(gc::alg::AlgorithmStorage& s) const
        -> gc::NodeActivationAlgorithms override
    {
        namespace a = gc::alg;

        auto result = gc::NodeActivationAlgorithms{};

        // Declare types and symbols

        auto lib =
            s(a::Lib{ .name = "agc_app" });

        auto threshold_alg_header =
            s(a::HeaderFile{
                .name = "agc_app/alg/threshold.hpp",
                .lib = lib });

        auto threshold_func =
            s(a::Symbol{
                .name = "agc_app::threshold",
                .header_file = threshold_alg_header });

        // Bind input

        auto threshold = s(a::Var{ a::id::TypeFromBinding{} });
        auto value = s(a::Var{ a::id::TypeFromBinding{} });

        result.input_bindings = {
            s(a::InputBinding{ .port = 0_gc_i, .var = threshold }),
            s(a::InputBinding{ .port = 1_gc_i, .var = value })
        };

        // Define activation algorithms

        auto activate_threshold_statement =
            s(a::Statement{ s(a::Block{}) });

        result.algorithms.emplace_back(gc::PortActivationAlgorithm{
            .activate = activate_threshold_statement,
        });

        auto activate_value_statement =
            s(a::Statement{ s(a::If{
                .condition = s(a::FuncInvocation{
                    .func = threshold_func,
                    .args = s(a::Vars{value, threshold}) }),
            .then_clause = s(a::Statement{ s(a::OutputActivation{
                .port = 0_gc_o,
                .var = value }) }),
            .else_clause = s(a::Statement{ s(a::OutputActivation{
                .port = 1_gc_o,
                .var = value }) })
            }) });

        result.algorithms.emplace_back(gc::PortActivationAlgorithm{
            .required_inputs = {0_gc_i},
            .activate = activate_value_statement,
        });

        // `threshold` constitutes node state, no need to specify additional
        // `result.context`.

        return result;
    }

    GCLIB_DECL_ACTIVATION_NODE_META(Threshold);
};

} // anonymous namespace


auto make_threshold(gc::ConstValueSpan args)
    -> std::shared_ptr<gc::ActivationNode>
{
    gc::expect_no_node_args(Threshold::static_meta().type_name, args);
    return std::make_shared<Threshold>();
}

} // namespace agc_app
