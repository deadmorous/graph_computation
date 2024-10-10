#include "gc_app/rect_view.hpp"

#include "gc_app/image.hpp"

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
    { return gc::node_input_names<RectView>( "size"sv, "sequence"sv ); }

    auto output_names() const
        -> common::ConstNameSpan
    { return gc::node_output_names<RectView>( "image"sv ); }

    auto default_inputs(gc::ValueSpan result) const
        -> void
    {
        assert(result.size() == 2);
        result[0] = UintSize(100, 100);
        result[1] = UintVec(10000, 1);
    }

    auto compute_outputs(
            gc::ValueSpan result,
            gc::ConstValueSpan inputs) const
        -> void
    {
        assert(inputs.size() == 2);
        assert(result.size() == 1);
        const auto& size = inputs[0].as<UintSize>();
        const auto& seq = inputs[1].as<UintVec>();
        auto image = Image
        {
            .size = size,
            .data = UintVec(size.width * size.height, rgba(0, 0))
        };

        auto N = *std::max_element(seq.begin(), seq.end());
        auto d = 0xff / N;

        auto n = std::min(image.data.size(), seq.size());
        for(size_t index=0; index<n; ++index)
        {
            auto value = seq[index];
            if (value != 0)
            {
                // *pixel = rgba(0xff, 0xff, 0xff);
                auto v = value*d;
                image.data[index] = rgba(0xff, 0xff-v, 0xff-v);
            }
        }

        result[0] = std::move(image);
    }
};

auto make_rect_view()
    -> std::shared_ptr<gc::Node>
{ return std::make_shared<RectView>(); }

} // namespace gc_app
