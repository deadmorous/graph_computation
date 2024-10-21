#include "gc_app/rect_view.hpp"

#include "gc_app/image.hpp"
#include "gc_app/palette.hpp"

#include "gc/expect_n_node_args.hpp"
#include "gc/node.hpp"
#include "gc/node_port_names.hpp"


using namespace std::string_view_literals;

namespace gc_app {

class RectView final :
    public gc::Node
{
public:
    auto input_names() const
        -> common::ConstNameSpan
    {
        return gc::node_input_names<RectView>(
            "size"sv, "sequence"sv, "palette"sv);
    }

    auto output_names() const
        -> common::ConstNameSpan
    { return gc::node_output_names<RectView>( "image"sv ); }

    auto default_inputs(gc::ValueSpan result) const
        -> void
    {
        assert(result.size() == 3);
        result[0] = UintSize(100, 100);
        result[1] = UintVec(10000, 1);
        result[2] = IndexedPalette{
            .color_map = { rgba(Color{0xffffff}) },
            .overflow_color = rgba(Color{0})
        };
    }

    auto compute_outputs(
            gc::ValueSpan result,
            gc::ConstValueSpan inputs) const
        -> void
    {
        assert(inputs.size() == 3);
        assert(result.size() == 1);
        const auto& size = inputs[0].as<UintSize>();
        const auto& seq = inputs[1].as<UintVec>();
        const auto& palette = inputs[2].as<IndexedPalette>();
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
        }

        result[0] = std::move(image);
    }
};

auto make_rect_view(gc::ConstValueSpan args)
    -> std::shared_ptr<gc::Node>
{
    gc::expect_no_node_args("RectView", args);
    return std::make_shared<RectView>();
}

} // namespace gc_app
