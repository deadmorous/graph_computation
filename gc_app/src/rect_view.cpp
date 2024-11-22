#include "gc_app/rect_view.hpp"

#include "gc_app/image.hpp"
#include "gc_app/palette.hpp"

#include "gc/expect_n_node_args.hpp"
#include "gc/node.hpp"
#include "gc/node_port_names.hpp"


using namespace std::string_view_literals;
using namespace gc::literals;

namespace gc_app {

class RectView final :
    public gc::Node
{
public:
    auto input_names() const
        -> common::ConstNameSpan override
    {
        return gc::node_input_names<RectView>(
            "size"sv, "sequence"sv, "palette"sv);
    }

    auto output_names() const
        -> common::ConstNameSpan override
    { return gc::node_output_names<RectView>( "image"sv ); }

    auto default_inputs(gc::InputValues result) const
        -> void override
    {
        assert(result.size() == 3_gc_ic);
        result[0_gc_i] = UintSize(100, 100);
        result[1_gc_i] = UintVec(10000, 1);
        result[2_gc_i] = IndexedPalette{
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
        assert(inputs.size() == 3_gc_ic);
        assert(result.size() == 1_gc_oc);
        const auto& size = inputs[0_gc_i].as<UintSize>();
        const auto& seq = inputs[1_gc_i].as<UintVec>();
        const auto& palette = inputs[2_gc_i].as<IndexedPalette>();
        auto image = Image
        {
            .size = size,
            .data = ColorVec(size.width * size.height,
                             rgba(Color{0}, ColorComponent{0}))
        };

        auto n = std::min(image.data.size(), seq.size());
        for(size_t index=0; index<n; ++index)
        {
            auto value = seq[index];
            image.data[index] = map_color(palette, value);
            if (stoken.stop_requested())
                return false;
        }

        result.front() = std::move(image);
        return true;
    }
};

auto make_rect_view(gc::ConstValueSpan args)
    -> std::shared_ptr<gc::Node>
{
    gc::expect_no_node_args("RectView", args);
    return std::make_shared<RectView>();
}

} // namespace gc_app
