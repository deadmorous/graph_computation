#include "agc_app/nodes/scale.hpp"

#include "agc_app/alg_lib.hpp"

#include "gc/algorithm.hpp"
#include "gc/activation_node.hpp"

#include "gc/expect_n_node_args.hpp"
#include "gc/node_port_names.hpp"
#include "gc/value.hpp"


using namespace gc::literals;
using namespace std::string_view_literals;

namespace agc_app {
namespace {

class Scale final :
    public gc::ActivationNode
{
public:
    auto input_names() const
        -> gc::InputNames override
    { return gc::node_input_names<Scale>("factor"sv, "value"sv); }

    auto output_names() const
        -> gc::OutputNames override
    { return gc::node_output_names<Scale>("scaled"sv); }

    auto activation_algorithms(gc::alg::AlgorithmStorage& s) const
        -> gc::NodeActivationAlgorithms override
    {
        namespace a = gc::alg;

        auto result = gc::NodeActivationAlgorithms{};

        // Declare types and symbols

        auto lib = alg_lib(s);

        auto scale_alg_header =
            s(a::HeaderFile{
                .name = "agc_app/alg/scale.hpp",
                .lib = lib });

        auto scale_func =
            s(a::Symbol{
                .name = "agc_app::scale",
                .header_file = scale_alg_header });

        // Bind inputs

        auto factor = s(a::Var{ a::id::TypeFromBinding{} });
        auto value = s(a::Var{ a::id::TypeFromBinding{} });

        result.input_bindings = {
            s(a::InputBinding{ .port = 0_gc_i, .var = factor }),
            s(a::InputBinding{ .port = 1_gc_i, .var = value })
        };

        // Define activation algorithms

        auto scale_args =
            s(a::Vars{factor, value});

        auto output_port_value_init =
            s(a::FuncInvocation{
                .func = scale_func,
                .args = scale_args });

        auto output_port_value =
            s(a::Var{ output_port_value_init });

        auto activate_factor_statement =
            s(a::Statement{ s(a::Block{}) });

        result.algorithms.emplace_back(gc::PortActivationAlgorithm{
            .activate = activate_factor_statement,
        });

        auto activate_value_statement =
            s(a::Statement{ s(a::Block{
                .vars = s(a::Vars{output_port_value}),
                .statements = {
                    s(a::Statement{ s(a::OutputActivation{
                        .port = 0_gc_o,
                        .var = output_port_value }) }),
                } }) });

        result.algorithms.emplace_back(gc::PortActivationAlgorithm{
            .required_inputs = {0_gc_i},
            .activate = activate_value_statement,
        });

        // No context is required, node is stateless -
        // not setting `result.context`.

        return result;
    }

    GCLIB_DECL_ACTIVATION_NODE_META(Scale);
};

} // anonymous namespace


auto make_scale(gc::ConstValueSpan args)
    -> std::shared_ptr<gc::ActivationNode>
{
    gc::expect_no_node_args(Scale::static_meta().type_name, args);
    return std::make_shared<Scale>();
}

} // namespace agc_app
