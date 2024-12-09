#include "agc_app/linspace.hpp"

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
        return gc::node_input_names<LinSpace>(
            "first"sv, "last"sv, "count"sv, "trigger"sv );
    }

    auto output_names() const
        -> gc::OutputNames override
    { return gc::node_output_names<LinSpace>( "sequence"sv ); }

    auto default_inputs(gc::InputValues result) const
        -> void override
    {
        assert(result.size() == 4_gc_ic);
        result[0_gc_i] = 0.;
        result[1_gc_i] = 1.;
        result[2_gc_i] = 11;
    }

    auto activation_algorithms(gc::ActivationAlgorithmsResult result,
                               gc::alg::AlgorithmStorage& alg_storage) const
        -> void override
    {
        assert(result.size() == 1_gc_ic);

        auto lib = alg_storage(gc::alg::Lib{ .name = "agc_app" });
        auto header =
            alg_storage(gc::alg::HeaderFile{ .name = "alg/linspace.hpp" });
        auto context_type =
            alg_storage(gc::alg::Type{ .name = "LinspaceState",
                                       .header_file = header });
        auto context = alg_storage(gc::alg::Var{ .type = context_type });

        result[0_gc_i] =
        {
            .required_inputs = {},
            // .activate = save_port(0_gc_i),
            .context = context
        };
//        result0.required_inputs = gc::InputPorts{0_gc_i, 1_gc_i, 2_gc_i};
    }
};

auto make_linspace(gc::ConstValueSpan args)
    -> std::shared_ptr<gc::ActivationNode>
{
    gc::expect_no_node_args("LinSpace", args);
    return std::make_shared<LinSpace>();
}

} // namespace agc_app
