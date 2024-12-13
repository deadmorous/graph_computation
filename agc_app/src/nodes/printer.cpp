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

    auto default_inputs(gc::InputValues result) const
        -> void override
    {
        assert(result.size() == 1_gc_ic);
        result[0_gc_i] = 0;
    }

    auto activation_algorithms(gc::alg::AlgorithmStorage& s) const
        -> gc::NodeActivationAlgorithms override
    {
        namespace a = gc::alg;

        auto result = gc::NodeActivationAlgorithms{};

        // Declare types

        auto lib =
            s(a::Lib{ .name = "agc_app" });

        auto printer_alg_header =
            s(a::HeaderFile{
                .name = "agc_app/alg/printer.hpp",
                .lib = lib });

        // Bind input

        auto value =
            s(a::Var{ gc::alg::id::TypeFromBinding{} });

        result.input_bindings = {
            s(a::InputBinding{ .port = 0_gc_i, .var = value })
        };

        // Declare functions

        auto print_func =
            s(a::Symbol{
                .name = "gc_app::print",
                .header_file = printer_alg_header });

        // Define ativation algorithm

        auto activate_statement =
            s(a::Statement{ s(a::FuncInvocation{
                .func = print_func,
                .args = s(a::Vars{value}) }) });

        result.algorithms.emplace_back(gc::PortActivationAlgorithm{
            .required_inputs = {0_gc_i},
            .activate = activate_statement,
            .context = {}   // No context is required, node is stateless
        });

        return result;
    }
};

auto make_printer(gc::ConstValueSpan args)
    -> std::shared_ptr<gc::ActivationNode>
{
    gc::expect_no_node_args("Printer", args);
    return std::make_shared<Printer>();
}

} // namespace agc_app
