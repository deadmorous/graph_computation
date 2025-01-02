#include "agc_app/nodes/canvas.hpp"
#include "agc_app/types/canvas.hpp"

#include "gc/algorithm.hpp"
#include "gc/activation_node.hpp"

#include "gc/expect_n_node_args.hpp"
#include "gc/node_port_names.hpp"
#include "gc/value.hpp"


using namespace gc::literals;
using namespace std::string_view_literals;

namespace agc_app {
namespace {

class CanvasNode final :
    public gc::ActivationNode
{
public:
    auto input_names() const
        -> gc::InputNames override
    {
        return gc::node_input_names<CanvasNode>(
            "size"sv, "set"sv, "set_next"sv, "flush"sv, "clear"sv);
    }

    auto output_names() const
        -> gc::OutputNames override
    { return gc::node_output_names<CanvasNode>("canvas"sv); }

    auto default_inputs(gc::InputValues result) const
        -> void override
    {
        assert(result.size() == 4_gc_ic);
        result[0_gc_i] = std::array<uint32_t, 2>{ 320, 200 };
        result[1_gc_i] = CanvasPixel<double>{ {0, 0}, 0.5 };
        result[2_gc_i] = 0.5;
        result[3_gc_i] = 0;
        result[4_gc_i] = 0;
    }

    auto activation_algorithms(gc::alg::AlgorithmStorage& s) const
        -> gc::NodeActivationAlgorithms override
    {
        namespace a = gc::alg;

        auto result = gc::NodeActivationAlgorithms{};

        // Declare types and symbols

        auto lib =
            s(a::Lib{ .name = "agc_app" });

        auto canvas_type_header =
            s(a::HeaderFile{
                .name = "agc_app/types/canvas.hpp",
                .lib = lib });

        auto canvas_alg_header =
            s(a::HeaderFile{
                .name = "agc_app/alg/canvas.hpp",
                .lib = lib });

        auto size_t_header =
            s(a::HeaderFile{
                .name = "cstddef",
                .system = true });

        auto canvas_type =
            s(a::Type{
                .name = "agc_app::Canvas<double>",
                .header_file = canvas_type_header });

        auto canvas_pixel_type =
            s(a::Type{
                .name = "agc_app::CanvasPixel<double>",
                .header_file = canvas_type_header });

        auto size_t_type =
            s(a::Type{
                .name = "size_t",
                .header_file = size_t_header });

        auto canvas_size_type =
            s(a::Type{
                .name = "std::array<uint32_t, 2>",
                .header_file = canvas_type_header });

        auto double_type =
            s(a::Type{ .name = "double" });


        auto resize_canvas_func =
            s(a::Symbol{
                .name = "gc_app::resize_canvas",
                .header_file = canvas_alg_header });

        auto clear_canvas_func =
            s(a::Symbol{
                .name = "gc_app::clear_canvas",
                .header_file = canvas_alg_header });

        auto set_canvas_pixel_func =
            s(a::Symbol{
                .name = "gc_app::set_canvas_pixel",
                .header_file = canvas_alg_header });

        auto set_next_canvas_pixel_func =
            s(a::Symbol{
                .name = "gc_app::set_next_canvas_pixel",
                .header_file = canvas_alg_header });

        // Define context variables

        auto canvas =
            s(a::Var{ canvas_type });

        auto pixel_index =
            s(a::Var{ size_t_type });

        // Bind input

        auto canvas_size =
            s(a::Var{ canvas_size_type });

        auto pixel =
            s(a::Var{ canvas_pixel_type });

        auto next_value =
            s(a::Var{ double_type });

        result.input_bindings = {
            s(a::InputBinding{ .port = 0_gc_i, .var = canvas_size }),
            s(a::InputBinding{ .port = 1_gc_i, .var = pixel }),
            s(a::InputBinding{ .port = 2_gc_i, .var = next_value })
        };

        // Define activation algorithm

        auto activate_size_statement =
            s(a::Statement{ s(a::FuncInvocation{
                .func = resize_canvas_func,
                .args = s(a::Vars{canvas_size}) }) });

        result.algorithms.emplace_back(gc::PortActivationAlgorithm{
            .activate = activate_size_statement,
        });

        auto activate_set_statement =
            s(a::Statement{ s(a::FuncInvocation{
                .func = set_canvas_pixel_func,
                .args = s(a::Vars{canvas, pixel}) }) });

        result.algorithms.emplace_back(gc::PortActivationAlgorithm{
            .required_inputs = {0_gc_i},
            .activate = activate_set_statement,
        });

        auto activate_set_next_statement =
            s(a::Statement{ s(a::FuncInvocation{
                .func = set_next_canvas_pixel_func,
                .args = s(a::Vars{canvas, next_value}) }) });

        result.algorithms.emplace_back(gc::PortActivationAlgorithm{
            .required_inputs = {0_gc_i},
            .activate = activate_set_next_statement,
        });

        auto activate_flush_statement =
            s(a::Statement{ s(a::OutputActivation{
                .port = 0_gc_o,
                .var = canvas }) });

        result.algorithms.emplace_back(gc::PortActivationAlgorithm{
            .activate = activate_flush_statement,
        });

        auto activate_clear_statement =
            s(a::Statement{ s(a::FuncInvocation{
                .func = clear_canvas_func,
                .args = s(a::Vars{canvas}) }) });

        result.algorithms.emplace_back(gc::PortActivationAlgorithm{
            .activate = activate_clear_statement,
        });

        // A context is required, set `result.context`.
        result.context = s(a::Vars{canvas, pixel_index});

        return result;
    }
};

} // anonymous namespace


auto make_canvas(gc::ConstValueSpan args)
    -> std::shared_ptr<gc::ActivationNode>
{
    gc::expect_no_node_args("Canvas", args);
    return std::make_shared<CanvasNode>();
}

} // namespace agc_app
