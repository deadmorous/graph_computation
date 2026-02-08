/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025-2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#include "gc_app/nodes/cell_aut/offset_image.hpp"

#include "gc_types/image.hpp"

#include "gc/expect_n_node_args.hpp"
#include "gc/computation_node.hpp"
#include "gc/node_port_names.hpp"

#include "common/func_ref.hpp"

#include <cassert>
#include <random>


using namespace std::string_view_literals;
using namespace gc::literals;

namespace gc_app::cell_aut {

using namespace gc_types;

class OffsetImage final :
    public gc::ComputationNode
{
public:
    auto input_names() const
        -> gc::InputNames override
    {
        return gc::node_input_names<OffsetImage>(
            "input_image"sv,
            "offset"sv);
    }

    auto output_names() const
        -> gc::OutputNames override
    { return gc::node_output_names<OffsetImage>( "output_image"sv ); }

    auto default_inputs(gc::InputValues result) const
        -> void override
    {
        assert(result.size() == 2_gc_ic);
        result[0_gc_i] = I8Image{
            .size = {100, 100},
            .data = std::vector<int8_t>(100*100, 0)
        };
        result[1_gc_i] = int8_t{-1};
    }

    auto compute_outputs(
            gc::OutputValues result,
            gc::ConstInputValues inputs,
            const std::stop_token&,
            const gc::NodeProgress& progress) const
        -> bool override
    {
        assert(inputs.size() == 2_gc_ic);
        assert(result.size() == 1_gc_oc);
        const auto& input_image = inputs[0_gc_i].as<I8Image>();
        auto offset = inputs[1_gc_i].convert_to<int8_t>();

        auto& output_image = [&]() -> I8Image&
        {
            static const auto* I8Image_type = gc::type_of<I8Image>();
            auto& out = result.front();
            if (out.type() == I8Image_type)
            {
                auto& image = out.as<I8Image>();
                if (image.size == input_image.size)
                    return image;
            }
            out = I8Image{
                .size = input_image.size,
                .data = std::vector<int8_t>(input_image.data.size())
            };
            return out.as<I8Image>();
        }();

        std::ranges::transform(
            input_image.data, output_image.data.begin(),
            [&](int8_t pixel){ return pixel + offset; });

        if (progress)
            progress(1);

        return true;
    }

private:
    static auto generate_image(
        const UintSize& size,
        int8_t lowest_state,
        int8_t range_size,
        const std::vector<int8_t>& map) -> I8Image
    {
        if (range_size == 0)
            throw std::invalid_argument(
                "OffsetImage: range_size must be positive");
        if (size.width < 1 || size.height < 1)
            throw std::invalid_argument(
                "OffsetImage: image width and height must both be positive");

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int8_t> distrib(
            lowest_state, lowest_state + range_size - 1);

        auto image = I8Image{
            .size = size,
            .data = std::vector<int8_t>(size.width * size.height, 0)
        };
        for (auto& pixel : image.data)
            pixel = distrib(gen);

        if( !map.empty() )
        {
            if (map.size() != static_cast<unsigned int>(range_size))
                throw std::invalid_argument(
                    "OffsetImage: Map size must be either zero or range_size");

            // Do the mapping
            for (auto& pixel : image.data)
                pixel = map.at(pixel - lowest_state) + lowest_state;
        }

        return image;
    }

};

auto make_offset_image(gc::ConstValueSpan args, const gc::ComputationContext&)
    -> std::shared_ptr<gc::ComputationNode>
{
    gc::expect_no_node_args("OffsetImage", args);
    return std::make_shared<OffsetImage>();
}

} // namespace gc_app::cell_aut
