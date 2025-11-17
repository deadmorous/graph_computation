/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#include "gc_app/nodes/cell_aut/generate_cmap.hpp"

#include "gc_app/types/cell2d_gen_cmap.hpp"
#include "gc_app/types/color.hpp"
#include "gc_app/types/palette.hpp"

#include "gc/expect_n_node_args.hpp"
#include "gc/computation_node.hpp"
#include "gc/node_port_names.hpp"
#include "gc/value.hpp"

#include "common/expr_calculator.hpp"
#include "common/func_ref.hpp"

#include <cassert>
#include <cstring>


using namespace std::literals;
using namespace gc::literals;

namespace gc_app::cell_aut {

namespace {

auto generate_cmap(const Cell2dGenCmap& gen_cmap) -> IndexedColorMap
{
    using U8Vec = std::vector<uint8_t>;
    using RgbComponentVecs = Cell2dGenCmap_Rgb<U8Vec>;

    const int max = gen_cmap.state_count - 1;

    auto fill_map = [&](
        U8Vec& map,
        const std::string& formula,
        const Cell2dIndexRange& range,
        const std::string& context)
    {
        try {
            auto calc = common::ExprCalculator{formula};
            auto variables = std::unordered_map<std::string_view, double>{};
            auto& n_var = variables["n"sv];
            auto step = std::max(range.step, 1);
            for(int n=range.min; n<=range.max; n+=step)
            {
                n_var = n;
                map.at(n) = calc(variables);    // TODO: Clamp & round
            }
        }
        catch(std::exception& e) {
            common::throw_("generate_cmap: ", context, ": ", e.what());
        }
    };

    auto fill_rgb_maps = [&](
        RgbComponentVecs& maps,
        const Cell2dGenCmap::Overlay& overlay,
        const std::string& context)
    {
        fill_map(maps.r, overlay.formula.r, overlay.range, context + ", red");
        fill_map(maps.g, overlay.formula.g, overlay.range, context + ", green");
        fill_map(maps.b, overlay.formula.b, overlay.range, context + ", blue");
    };

    auto test_map = [&](
        const U8Vec& map,
        const std::string& context)
    {

        for(auto i : common::index_range<size_t>(map.size()))
        {
            if((map[i] > max || map[i] < 0))
            {
                common::throw_(
                    "generate_cmap: ", context, ": Map element ", i,
                    " is out of range (==", int(map[i]), ")");
            }
        }
    };

    auto test_rgb_maps = [&](
        const RgbComponentVecs& maps)
    {
        test_map(maps.r, "red");
        test_map(maps.g, "green");
        test_map(maps.b, "blue");
    };

    auto rgb_maps = RgbComponentVecs{
        .r = U8Vec(gen_cmap.state_count, 0),
        .g = U8Vec(gen_cmap.state_count, 0),
        .b = U8Vec(gen_cmap.state_count, 0) };

    fill_rgb_maps(
        rgb_maps, {
            .formula = gen_cmap.formula,
            .range = {
                .min = 0,
                .max = max,
                .step = 1 }},
        "main formula");

    for (size_t overlay_index=0; auto& overlay : gen_cmap.overlays)
        fill_rgb_maps(
            rgb_maps,
            overlay,
            common::format("overlay ", overlay_index++));

    test_rgb_maps(rgb_maps);

    auto result = IndexedColorMap(gen_cmap.state_count);
    using C = ColorComponent;
    for (auto i : common::index_range<size_t>(gen_cmap.state_count))
        result[i] = rgba(C{rgb_maps.r[i]}, C{rgb_maps.g[i]}, C{rgb_maps.b[i]});

    return result;
}

} // anonymous namespace

class GenerateCmap final :
    public gc::ComputationNode
{
public:
    auto input_names() const
        -> gc::InputNames override
    {
        return gc::node_input_names<GenerateCmap>("gen_cmap"sv);
    }

    auto output_names() const
        -> gc::OutputNames override
    { return gc::node_output_names<GenerateCmap>("cmap"sv); }

    auto default_inputs(gc::InputValues result) const
        -> void override
    {
        assert(result.size() == 1_gc_ic);
        result[0_gc_i] = Cell2dGenCmap{};
    }

    auto compute_outputs(
            gc::OutputValues result,
            gc::ConstInputValues inputs,
            const std::stop_token& stoken,
            const gc::NodeProgress& progress) const
        -> bool override
    {
        assert(inputs.size() == 1_gc_ic);
        assert(result.size() == 1_gc_oc);
        const auto& gen_cmap = inputs[0_gc_i].as<Cell2dGenCmap>();
        result[0_gc_o] = generate_cmap(gen_cmap);

        if (progress)
            progress(1);

        return true;
    }
};

auto make_generate_cmap(gc::ConstValueSpan args, const gc::ComputationContext&)
    -> std::shared_ptr<gc::ComputationNode>
{
    gc::expect_no_node_args("GenerateCmap", args);
    return std::make_shared<GenerateCmap>();
}

} // namespace gc_app::cell_aut
