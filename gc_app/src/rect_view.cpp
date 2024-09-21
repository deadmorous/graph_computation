#include "gc_app/rect_view.hpp"

#include "gc_app/image.hpp"


namespace gc_app {

auto RectView::input_count() const
    -> uint32_t
{ return 2; }

auto RectView::output_count() const
    -> uint32_t
{ return 1; }

auto RectView::default_inputs(gc::ValueSpan result) const
    -> void
{
    assert(result.size() == 2);
    result[0] = UintSize(100, 100);
    result[1] = UintVec(10000, 1);
}

auto RectView::compute_outputs(
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

} // namespace gc_app
