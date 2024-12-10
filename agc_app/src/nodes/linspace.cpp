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

    auto activation_algorithms(gc::ActivationAlgorithmsResult result,
                               gc::alg::AlgorithmStorage& alg_storage) const
        -> void override
    {
        namespace a = gc::alg;

        assert(result.algorithms.size() == 1_gc_ic);

        auto lib =
            alg_storage(a::Lib{ .name = "agc_app" });

        auto linspace_spec_header =
            alg_storage(a::HeaderFile{
                .name = "agc_app/types/linspace_spec.hpp",
                .lib = lib });

        auto spec_type =
            alg_storage(a::Type{
                .name = "gc_app::LinSpaceSpec",
                .header_file = linspace_spec_header });

        auto double_type =
            alg_storage(a::Type{ .name = "double" });

        auto cstdint_header =
            alg_storage(a::HeaderFile{ .name = "cstdint", .system = true });

        auto u32_type =
            alg_storage(a::Type{
                .name = "uint32_t",
                .header_file = cstdint_header });

        auto spec =
            alg_storage(a::Var{ .type = spec_type });

        auto context_val =
            alg_storage(a::Var{ .type = double_type });

        auto context_index =
            alg_storage(a::Var{ .type = u32_type });

        auto context = alg_storage(a::Vars{ context_index, context_val });

        auto out_activation =
            alg_storage(a::OutputActivation{
                .port = 0_gc_o,
                .var = context_val });

        result.input_bindings = {
            alg_storage(a::InputBinding{ .port = 0_gc_i, .var = spec })
        };

        result.algorithms[0_gc_i] =
        {
            .required_inputs = {0_gc_i},
            // .activate = save_port(0_gc_i),
            .context = context
        };
    }
};

auto make_linspace(gc::ConstValueSpan args)
    -> std::shared_ptr<gc::ActivationNode>
{
    gc::expect_no_node_args("LinSpace", args);
    return std::make_shared<LinSpace>();
}

} // namespace agc_app
