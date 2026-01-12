/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "agc_app/nodes/mag2.hpp"

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

class Mag2 final :
    public gc::ActivationNode
{
public:
    auto input_names() const
        -> gc::InputNames override
    { return gc::node_input_names<Mag2>("value"sv); }

    auto output_names() const
        -> gc::OutputNames override
    { return gc::node_output_names<Mag2>("mag2"sv); }

    auto exported_types(gc::ExportedTypes& xt,
                        gc::alg::AlgorithmStorage& s) const
        -> void override
    {
        // Declare exported types

        namespace a = gc::alg;

        [[maybe_unused]] auto lib = alg_lib(s); // TODO: Do we need this?

        auto std_array_header =
            s(a::HeaderFile{
                .name = "array",
                .system = true });

        xt[mag2_input_type] =
            s(a::Type{
                .name = "std::array<double, 2>",
                .header_file = std_array_header });

        xt[mag2_output_type] = a::well_known_type(a::double_type, s);
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
                .name = "agc_app_rt/nodes/mag2.hpp",
                .lib = lib });

        auto mag2_func =
            s(a::Symbol{
                .name = "agc_app_rt::mag2",
                .header_file = mag2_alg_header });

        // Bind input

        auto value = s(a::Var{ xt.at(mag2_input_type) });

        result.input_bindings = {
            s(a::InputBinding{ .port = 0_gc_i, .var = value })
        };

        // Define activation algorithm

        auto mag2_args =
            s(a::Vars{value});

        auto output_port_value_init =
            s(a::FuncInvocation{
                .func = mag2_func,
                .args = mag2_args });

        auto output_port_value =
            s(a::Var{ output_port_value_init });

        auto activate_statement =
            s(a::Statement{ s(a::Block{
                .vars = s(a::Vars{output_port_value}),
                .statements = {
                    s(a::Statement{ s(a::OutputActivation{
                        .port = 0_gc_o,
                        .var = output_port_value }) }),
                } }) });

        result.algorithms.emplace_back(gc::PortActivationAlgorithm{
            .required_inputs = {0_gc_i},
            .activate = activate_statement,
        });

        // No context is required, node is stateless -
        // not setting `result.context`.

        return result;
    }

    GCLIB_DECL_ACTIVATION_NODE_META(Mag2);
};

} // anonymous namespace


auto make_mag2(gc::ConstValueSpan args, const gc::ActivationContext&)
    -> std::shared_ptr<gc::ActivationNode>
{
    gc::expect_no_node_args(Mag2::static_meta().type_name, args);
    return std::make_shared<Mag2>();
}

} // namespace agc_app
