/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024-2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "agc_app/nodes/grid_2d.hpp"

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

class Grid2d final :
    public gc::ActivationNode
{
public:
    auto input_names() const
        -> gc::InputNames override
    {
        return gc::node_input_names<Grid2d>("spec"sv);
    }

    auto output_names() const
        -> gc::OutputNames override
    { return gc::node_output_names<Grid2d>("grid_size"sv, "point"sv, "end"sv); }

    auto exported_types(gc::ExportedTypes& xt,
                        gc::alg::AlgorithmStorage& s) const
        -> void override
    {
        // Declare exported types

        namespace a = gc::alg;

        auto lib = alg_lib(s);

        auto grid_2d_spec_header =
            s(a::HeaderFile{
                .name = "agc_app_rt/types/grid_2d_spec.hpp",
                .lib = lib });

        xt[grid_2d_spec_type] =
            s(a::Type{
                .name = "agc_app_rt::Grid2dSpec",
                .header_file = grid_2d_spec_header });
    }

    auto activation_algorithms(gc::alg::AlgorithmStorage& s) const
        -> gc::NodeActivationAlgorithms override
    {
        namespace a = gc::alg;

        auto result = gc::NodeActivationAlgorithms{};

        // Declare types and symbols

        const auto xt = ActivationNode::exported_types(s);

        auto lib = alg_lib(s);

        auto grid_2d_alg_header =
            s(a::HeaderFile{
                .name = "agc_app_rt/nodes/grid_2d.hpp",
                .lib = lib });

        auto spec_type = xt.at(grid_2d_spec_type);

        auto grid_size_func =
            s(a::Symbol{
                .name = "agc_app_rt::grid_2d_size",
                .header_file = grid_2d_alg_header });

        auto iter_init_func =
            s(a::Symbol{
                .name = "agc_app_rt::grid_2d_init_iter",
                .header_file = grid_2d_alg_header });

        auto iter_next_func =
            s(a::Symbol{
                .name = "agc_app_rt::grid_2d_next_iter",
                .header_file = grid_2d_alg_header });

        auto iter_deref_func =
            s(a::Symbol{
                .name = "agc_app_rt::grid_2d_deref_iter",
                .header_file = grid_2d_alg_header });

        // Bind input

        auto spec = s(a::Var{ spec_type });

        result.input_bindings = {
            s(a::InputBinding{ .port = 0_gc_i, .var = spec })
        };

        // Define activation algorithm

        auto spec_args =
            s(a::Vars{spec});

        auto iter_state_init =
            s(a::FuncInvocation{
                .func = iter_init_func,
                .args = spec_args });

        auto iter_state =
            s(a::Var{ iter_state_init });

        auto iter_state_args =
            s(a::Vars{iter_state});

        auto grid_size_init =
            s(a::FuncInvocation{
                .func = grid_size_func,
                .args = spec_args });

        auto grid_size =
            s(a::Var{ grid_size_init });

        auto output_port_value_init =
            s(a::FuncInvocation{
                .func = iter_deref_func,
                .args = iter_state_args });

        auto output_port_value =
            s(a::Var{ output_port_value_init });

        auto activate_statement =
            s(a::Statement{ s(a::Block{
                .vars = s(a::Vars{grid_size, iter_state}),
                .statements = {
                    s(a::Statement{ s(a::OutputActivation{
                        .port = 0_gc_o,
                        .var = grid_size }) }),
                    s(a::Statement{ s(a::Do{
                        .condition = s(a::FuncInvocation{
                            .func = iter_next_func,
                            .args = iter_state_args }),
                        .body = s(a::Statement{ s(a::Block{
                            .vars = s(a::Vars{ output_port_value }),
                            .statements = {
                                s(a::Statement{ s(a::OutputActivation{
                                    .port = 1_gc_o,
                                    .var = output_port_value
                                }) })
                            } }) })
                    }) }),
                    s(a::Statement{ s(a::OutputActivation{ .port = 2_gc_o }) })
                } }) });

        result.algorithms.emplace_back(gc::PortActivationAlgorithm{
            .required_inputs = {0_gc_i},
            .activate = activate_statement,
        });

        // No context is required, node is stateless -
        // not setting `result.context`.

        return result;
    }

    GCLIB_DECL_ACTIVATION_NODE_META(Grid2d);
};

} // anonymous namespace


auto make_grid_2d(gc::ConstValueSpan args, const gc::ActivationContext&)
    -> std::shared_ptr<gc::ActivationNode>
{
    gc::expect_no_node_args(Grid2d::static_meta().type_name, args);
    return std::make_shared<Grid2d>();
}

} // namespace agc_app
