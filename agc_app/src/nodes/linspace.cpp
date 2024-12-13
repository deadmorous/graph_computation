#include "agc_app/nodes/linspace.hpp"
#include "agc_app/types/linspace_spec.hpp"

#include "gc/algorithm.hpp"
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
        -> gc::InputNames override
    {
        return gc::node_input_names<LinSpace>("spec"sv);
    }

    auto output_names() const
        -> gc::OutputNames override
    { return gc::node_output_names<LinSpace>("sequence"sv); }

    auto default_inputs(gc::InputValues result) const
        -> void override
    {
        assert(result.size() == 1_gc_ic);
        result[0_gc_i] = LinSpaceSpec{};
    }

    auto activation_algorithms(gc::alg::AlgorithmStorage& s) const
        -> gc::NodeActivationAlgorithms override
    {
        namespace a = gc::alg;

        auto result = gc::NodeActivationAlgorithms{};

        // Declare types

        auto lib =
            s(a::Lib{ .name = "agc_app" });

        auto linspace_spec_header =
            s(a::HeaderFile{
                .name = "agc_app/types/linspace_spec.hpp",
                .lib = lib });

        auto linspace_alg_header =
            s(a::HeaderFile{
                .name = "agc_app/alg/linspace.hpp",
                .lib = lib });

        auto spec_type =
            s(a::Type{
                .name = "gc_app::LinSpaceSpec",
                .header_file = linspace_spec_header });

        auto iter_state_type =
            s(a::Type{
                .name = "gc_app::LinSpaceIterState",
                .header_file = linspace_alg_header });

        auto double_type =
            s(a::Type{ .name = "double" });

        // Bind input

        auto spec =
            s(a::Var{ spec_type });

        result.input_bindings = {
            s(a::InputBinding{ .port = 0_gc_i, .var = spec })
        };

        // Declare functions

        auto iter_init_func =
            s(a::Symbol{
                .name = "gc_app::LinSpaceInitIter",
                .header_file = linspace_alg_header });

        auto iter_next_func =
            s(a::Symbol{
                .name = "gc_app::LinSpaceNextIter",
                .header_file = linspace_alg_header });

        auto iter_deref_func =
            s(a::Symbol{
                .name = "gc_app::LinSpaceDerefIter",
                .header_file = linspace_alg_header });

        // Define ativation algorithm

        auto iter_state =
            s(a::Var{ iter_state_type });

        auto iter_state_args =
            s(a::Vars{iter_state});

        auto output_port_value =
            s(a::Var{ double_type });

        auto activate_statement =
            s(a::Statement{ s(a::Block{
                .vars = s(a::Vars{iter_state}),
                .statements = {
                    s(a::Statement{ s(a::FuncInvocation{
                        .func = iter_init_func,
                        .result = iter_state,
                        .args = s(a::Vars{spec}) }) }),
                    s(a::Statement{ s(a::While{
                        .condition = s(a::FuncInvocation{
                            .func = iter_next_func,
                            .args = iter_state_args }),
                        .body = s(a::Statement{ s(a::Block{
                            .statements = {
                                s(a::Statement{ s(a::FuncInvocation{
                                    .func = iter_deref_func,
                                    .result = output_port_value,
                                    .args = iter_state_args
                                }) }),
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
            .context = {}   // No context is required, node is stateless
        });

        return result;
    }
};

auto make_linspace(gc::ConstValueSpan args)
    -> std::shared_ptr<gc::ActivationNode>
{
    gc::expect_no_node_args("LinSpace", args);
    return std::make_shared<LinSpace>();
}

} // namespace agc_app
