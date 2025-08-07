#include "agc_app/nodes/linspace.hpp"

#include "agc_app/alg_lib.hpp"

#include "gc/algorithm.hpp"
#include "gc/activation_node.hpp"

#include "gc/expect_n_node_args.hpp"
#include "gc/node_port_names.hpp"
#include "gc/value.hpp"

#include <cassert>


using namespace gc::literals;
using namespace std::string_view_literals;

namespace agc_app {
namespace {

class LinSpace final :
    public gc::ActivationNode
{
public:
    auto input_names() const
        -> gc::InputNames override
    {
        return gc::node_input_names<LinSpace>("spec"sv);
    }

    auto output_names() const
        -> gc::OutputNames override
    { return gc::node_output_names<LinSpace>("sequence"sv); }

    auto exported_types(gc::ExportedTypes& xt,
                        gc::alg::AlgorithmStorage& s) const
        -> void override
    {
        // Declare exported types

        namespace a = gc::alg;

        auto lib = alg_lib(s);

        auto linspace_spec_header =
            s(a::HeaderFile{
                .name = "agc_app_rt/types/linspace_spec.hpp",
                .lib = lib });

        xt[linspace_spec_type] =
            s(a::Type{
                .name = "agc_app_rt::LinSpaceSpec",
                .header_file = linspace_spec_header });
    }

    auto activation_algorithms(gc::alg::AlgorithmStorage& s) const
        -> gc::NodeActivationAlgorithms override
    {
        namespace a = gc::alg;

        auto result = gc::NodeActivationAlgorithms{};

        // Declare types and symbols

        const auto xt = ActivationNode::exported_types(s);

        auto lib = alg_lib(s);

        auto linspace_alg_header =
            s(a::HeaderFile{
                .name = "agc_app_rt/alg/linspace.hpp",
                .lib = lib });

        auto iter_init_func =
            s(a::Symbol{
                .name = "agc_app_rt::LinSpaceInitIter",
                .header_file = linspace_alg_header });

        auto iter_next_func =
            s(a::Symbol{
                .name = "agc_app_rt::LinSpaceNextIter",
                .header_file = linspace_alg_header });

        auto iter_deref_func =
            s(a::Symbol{
                .name = "agc_app_rt::LinSpaceDerefIter",
                .header_file = linspace_alg_header });

        // Bind input

        auto spec =
            s(a::Var{ xt.at(linspace_spec_type) });

        result.input_bindings = {
            s(a::InputBinding{ .port = 0_gc_i, .var = spec })
        };

        // Define activation algorithm

        auto iter_state_init =
            s(a::FuncInvocation{
                .func = iter_init_func,
                .args = s(a::Vars{spec}) });

        auto iter_state =
            s(a::Var{ iter_state_init });


        auto iter_state_args =
            s(a::Vars{iter_state});

        auto output_port_value_init =
            s(a::FuncInvocation{
                .func = iter_deref_func,
                .args = iter_state_args });

        auto output_port_value =
            s(a::Var{ output_port_value_init });

        auto activate_statement =
            s(a::Statement{ s(a::Block{
                .vars = s(a::Vars{iter_state}),
                .statements = {
                    s(a::Statement{ s(a::Do{
                        .condition = s(a::FuncInvocation{
                            .func = iter_next_func,
                            .args = iter_state_args }),
                        .body = s(a::Statement{ s(a::Block{
                            .vars = s(a::Vars{ output_port_value }),
                            .statements = {
                                s(a::Statement{ s(a::OutputActivation{
                                    .port = 0_gc_o,
                                    .var = output_port_value
                                }) })
                            } }) })
                    }) })
                } }) });

        result.algorithms.emplace_back(gc::PortActivationAlgorithm{
            .required_inputs = {0_gc_i},
            .activate = activate_statement,
        });

        // No context is required, node is stateless -
        // not setting `result.context`.

        return result;
    }

    GCLIB_DECL_ACTIVATION_NODE_META(LinSpace);
};

} // anonymous namespace


auto make_linspace(gc::ConstValueSpan args)
    -> std::shared_ptr<gc::ActivationNode>
{
    gc::expect_no_node_args(LinSpace::static_meta().type_name, args);
    return std::make_shared<LinSpace>();
}

} // namespace agc_app
