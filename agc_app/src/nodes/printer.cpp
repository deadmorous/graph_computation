/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024-2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "agc_app/nodes/printer.hpp"

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

class Printer final :
    public gc::ActivationNode
{
public:
    auto input_names() const
        -> gc::InputNames override
    {
        return gc::node_input_names<Printer>("value"sv);
    }

    auto output_names() const
        -> gc::OutputNames override
    { return gc::node_output_names<Printer>(); }

    auto activation_algorithms(gc::alg::AlgorithmStorage& s) const
        -> gc::NodeActivationAlgorithms override
    {
        namespace a = gc::alg;

        auto result = gc::NodeActivationAlgorithms{};

        // Declare types and symbols

        auto lib = alg_lib(s);

        auto printer_alg_header =
            s(a::HeaderFile{
                .name = "agc_app/alg/printer.hpp",
                .lib = lib });

        auto print_func =
            s(a::Symbol{
                .name = "agc_app::print",
                .header_file = printer_alg_header });

        // Bind input

        auto value =
            s(a::Var{ a::id::TypeFromBinding{} });

        result.input_bindings = {
            s(a::InputBinding{ .port = 0_gc_i, .var = value })
        };

        // Define activation algorithm

        auto activate_statement =
            s(a::Statement{ s(a::FuncInvocation{
                .func = print_func,
                .args = s(a::Vars{value}) }) });

        result.algorithms.emplace_back(gc::PortActivationAlgorithm{
            .required_inputs = {0_gc_i},
            .activate = activate_statement,
        });

        // No context is required, node is stateless -
        // not setting `result.context`.

        return result;
    }

    GCLIB_DECL_ACTIVATION_NODE_META(Printer);
};

} // anonymous namespace


auto make_printer(gc::ConstValueSpan args)
    -> std::shared_ptr<gc::ActivationNode>
{
    gc::expect_no_node_args(Printer::static_meta().type_name, args);
    return std::make_shared<Printer>();
}

} // namespace agc_app
