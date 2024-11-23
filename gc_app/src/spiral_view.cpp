#include "gc_app/rect_view.hpp"

#include "gc_app/image.hpp"
#include "gc_app/palette.hpp"

#include "gc/expect_n_node_args.hpp"
#include "gc/computation_node.hpp"
#include "gc/node_port_names.hpp"

#include <cmath>
#include <numbers>


using namespace std::string_view_literals;
using namespace gc::literals;

namespace gc_app {

class SpiralView final :
    public gc::ComputationNode
{
public:
    auto input_names() const
        -> common::ConstNameSpan override
    {
        return gc::node_input_names<SpiralView>(
            "size"sv, "sequence"sv, "scale"sv, "palette"sv );
    }

    auto output_names() const
        -> common::ConstNameSpan override
    { return gc::node_output_names<SpiralView>( "image"sv ); }

    auto default_inputs(gc::InputValues result) const
        -> void override
    {
        assert(result.size() == 4_gc_ic);
        result[0_gc_i] = UintSize(100, 100);
        result[1_gc_i] = UintVec(10000, 1);
        result[2_gc_i] = 5.;
        result[3_gc_i] = IndexedPalette{
            .color_map = { rgba(Color{0xffffff}) },
            .overflow_color = rgba(Color{0})
        };
    }

    auto compute_outputs(
            gc::OutputValues result,
            gc::ConstInputValues inputs,
            const std::stop_token& stoken,
            const gc::NodeProgress& progress) const
        -> bool override
    {
        assert(inputs.size() == 4_gc_ic);
        assert(result.size() == 1_gc_oc);
        const auto& size = inputs[0_gc_i].as<UintSize>();
        const auto& seq = inputs[1_gc_i].as<UintVec>();
        auto scale = inputs[2_gc_i].convert_to<double>();
        const auto& palette = inputs[3_gc_i].as<IndexedPalette>();
        auto image = Image
        {
            .size = size,
            .data = ColorVec(size.width * size.height,
                             rgba(Color{0}, ColorComponent{0}))
        };

        auto n = std::min(image.data.size(), seq.size());

        auto c = 0.5 / std::numbers::pi * scale;
        auto c_2 = 0.5 * c;
        auto rx = size.width / 2;
        auto ry = size.height / 2;

        auto arc_length = [&](double phi)
        {
            auto sq_1p_phi2 = sqrt(1 + phi*phi);
            return c_2 * (phi*sq_1p_phi2 + log(sq_1p_phi2 + phi));
        };

        auto bg_color = average_color(palette, true);

        auto* pixel = image.data.data();
        for (uint32_t row=0; row<size.height; ++row)
        {
            if (stoken.stop_requested())
                return false;

            auto y = static_cast<double>(row) - ry;
            auto y2 = y*y;

            for (uint32_t col=0; col<size.width; ++col, ++pixel)
            {
                auto x = static_cast<double>(col) - rx;
                auto x2 = x*x;

                auto r = sqrt(x2 + y2);

                auto phi = atan2(y, x);
                if (phi < 0)
                    phi = 2*std::numbers::pi + phi;

                auto dr = c*phi;
                if (r > dr)
                    phi += 2*std::numbers::pi*floor((r-dr)/scale);

                auto s = arc_length(phi);
                auto index_d = s/scale + 2;
                auto index = static_cast<uint32_t>(index_d);

                if (index >= n)
                {
                    *pixel = rgba(ColorComponent{0xcc},
                                  ColorComponent{0xff},
                                  ColorComponent{0xcc});
                    continue;
                }

                auto value = seq[index];
                auto color = map_color(palette, value);

                if (scale <= 1)
                {
                    *pixel = color;
                    continue;
                }

                auto dL1 = fabs(index_d - index - 0.5) * 2;
                auto dL2 = fabs((c*phi - r) / (0.5*scale) + 1);
                double dL = sqrt(dL1*dL1 + dL2*dL2);

                *pixel = interp_color(color, bg_color, dL, 10);
            }
        }

        result.front() = std::move(image);
        return true;
    }
};

auto make_spiral_view(gc::ConstValueSpan args)
    -> std::shared_ptr<gc::ComputationNode>
{
    gc::expect_no_node_args("SpiralView", args);
    return std::make_shared<SpiralView>();
}

} // namespace gc_app
