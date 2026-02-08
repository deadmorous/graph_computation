/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025-2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "gc_app/nodes/visual/image_colorizer.hpp"

#include "gc_types/image.hpp"
#include "gc_types/palette.hpp"

#include "gc/computation_node.hpp"
#include "gc/expect_n_node_args.hpp"
#include "gc/node_port_names.hpp"


using namespace std::literals;
using namespace gc::literals;

namespace gc_app::visual {

using namespace gc_types;

class ImageColorizer final :
    public gc::ComputationNode
{
public:
    auto input_names() const
        -> gc::InputNames override
    {
        return gc::node_input_names<ImageColorizer>(
            "input_image"sv, "palette"sv, "min_state"sv);
    }

    auto output_names() const
        -> gc::OutputNames override
    { return gc::node_output_names<ImageColorizer>("output_image"sv); }

    auto default_inputs(gc::InputValues result) const
        -> void override
    {
        assert(result.size() == 3_gc_ic);
        result[0_gc_i] = Image<int8_t>{
            .size = {100, 100},
            .data = std::vector<int8_t>(100*100, 0)
        };
        using C = ColorComponent;
        result[1_gc_i] = IndexedPalette{
            .color_map = {
                rgba(C{0x00}, C{0x00}, C{0x00}),
                rgba(C{0xff}, C{0xff}, C{0xff}) },
            .overflow_color = rgba(C{0xcc}, C{0x00}, C{0x00})
        };
        result[2_gc_i] = int8_t{0};
    }

    auto compute_outputs(
            gc::OutputValues result,
            gc::ConstInputValues inputs,
            const std::stop_token&,
            const gc::NodeProgress&) const
        -> bool override
    {
        assert(inputs.size() == 3_gc_ic);
        assert(result.size() == 1_gc_oc);
        const auto& input_image = inputs[0_gc_i].as<Image<int8_t>>();
        const auto& palette = inputs[1_gc_i].as<IndexedPalette>();
        auto min_state = inputs[2_gc_i].convert_to<int8_t>();

        auto& output_image = [&]() -> ColorImage&
        {
            static const auto* ColorImage_type = gc::type_of<ColorImage>();
            auto& out = result.front();
            if (out.type() == ColorImage_type)
            {
                auto& image = out.as<ColorImage>();
                if (image.size == input_image.size)
                    return image;
            }
            out = ColorImage{
                .size = input_image.size,
                .data = std::vector<Color>(input_image.data.size())
            };
            return out.as<ColorImage>();
        }();

        const auto* input_pixel = input_image.data.data();
        auto* output_pixel = output_image.data.data();
        auto N = palette.color_map.size();
        for (auto _ : common::index_range<size_t>(input_image.data.size()))
        {
            auto in = *input_pixel++ - min_state;
            auto out = in >= 0 && static_cast<size_t>(in) < N
                           ? palette.color_map[in]
                           : palette.overflow_color;
            *output_pixel++ = out;
        }
        return true;
    }
};

auto make_image_colorizer(gc::ConstValueSpan args,
                          const gc::ComputationContext&)
    -> std::shared_ptr<gc::ComputationNode>
{
    gc::expect_no_node_args("ImageColorizer", args);
    return std::make_shared<ImageColorizer>();
}

} // namespace gc_app::visual
