/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "agc_app/nodes/mandelbrot_func.hpp"

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

class MandelbrotFunc final :
    public gc::ActivationNode
{
public:
    auto input_names() const
        -> gc::InputNames override
    { return gc::node_input_names<MandelbrotFunc>("c"sv, "z"sv); }

    auto output_names() const
        -> gc::OutputNames override
    { return gc::node_output_names<MandelbrotFunc>("value"sv); }

    auto exported_types(gc::ExportedTypes& xt,
                        gc::alg::AlgorithmStorage& s) const
        -> void override
    {
        // Declare exported types

        namespace a = gc::alg;

        auto lib = alg_lib(s);

        auto std_array_header =
            s(a::HeaderFile{
                .name = "array",
                .system = true });

        xt[mandelbrot_point_type] =
            s(a::Type{
                .name = "std::array<double, 2>",
                .header_file = std_array_header });
    }

    auto activation_algorithms(gc::alg::AlgorithmStorage& s) const
        -> gc::NodeActivationAlgorithms override
    {
        namespace a = gc::alg;

        auto result = gc::NodeActivationAlgorithms{};

        // Declare types and symbols

        const auto xt = ActivationNode::exported_types(s);
        auto lib = alg_lib(s);

        auto mag2_alg_header =
            s(a::HeaderFile{
                .name = "agc_app/alg/mandelbrot_func.hpp",
                .lib = lib });

        auto arg_type = xt.at(mandelbrot_point_type);

        auto mandelbrot_func =
            s(a::Symbol{
                .name = "agc_app::mandelbrot_func",
                .header_file = mag2_alg_header });

        // Bind input

        auto c = s(a::Var{ arg_type });
        auto z = s(a::Var{ arg_type });

        result.input_bindings = {
            s(a::InputBinding{ .port = 0_gc_i, .var = c }),
            s(a::InputBinding{ .port = 1_gc_i, .var = z })
        };

        // Define activation algorithms

        auto activate_c_statement =
            s(a::Statement{ s(a::Block{}) });

        result.algorithms.emplace_back(gc::PortActivationAlgorithm{
            .activate = activate_c_statement,
        });

        auto mandelbrot_func_args =
            s(a::Vars{c, z});

        auto output_port_value_init =
            s(a::FuncInvocation{
                .func = mandelbrot_func,
                .args = mandelbrot_func_args });

        auto output_port_value =
            s(a::Var{ output_port_value_init });

        auto activate_z_statement =
            s(a::Statement{ s(a::Block{
                .vars = s(a::Vars{output_port_value}),
                .statements = {
                    s(a::Statement{ s(a::OutputActivation{
                        .port = 0_gc_o,
                        .var = output_port_value }) }),
                } }) });

        result.algorithms.emplace_back(gc::PortActivationAlgorithm{
            .required_inputs = {0_gc_i},
            .activate = activate_z_statement,
        });

        // No context is required, node is stateless -
        // not setting `result.context`.

        return result;
    }

    GCLIB_DECL_ACTIVATION_NODE_META(MandelbrotFunc);
};

} // anonymous namespace


auto make_mandelbrot_func(gc::ConstValueSpan args)
    -> std::shared_ptr<gc::ActivationNode>
{
    gc::expect_no_node_args(MandelbrotFunc::static_meta().type_name, args);
    return std::make_shared<MandelbrotFunc>();
}

} // namespace agc_app
