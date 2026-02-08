/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025-2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#include "gc_app/nodes/cell_aut/random_image.hpp"

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

class RandomImage final :
    public gc::ComputationNode
{
public:
    auto input_names() const
        -> gc::InputNames override
    {
        return gc::node_input_names<RandomImage>(
            "size"sv,
            "lowest_state"sv,
            "range_size"sv,
            "map"sv,
            "radius"sv,
            "shape"sv,
            "outer_state"sv);
    }

    auto output_names() const
        -> gc::OutputNames override
    { return gc::node_output_names<RandomImage>( "image"sv ); }

    auto default_inputs(gc::InputValues result) const
        -> void override
    {
        assert(result.size() == 7_gc_ic);
        result[0_gc_i] = UintSize{100, 100};
        result[1_gc_i] = int8_t{0};
        result[2_gc_i] = int8_t{2};
        result[3_gc_i] = std::vector<int8_t>{};
        result[4_gc_i] = -1;
        result[5_gc_i] = std::string{"circle"};
        result[6_gc_i] = int8_t{0};
    }

    auto compute_outputs(
            gc::OutputValues result,
            gc::ConstInputValues inputs,
            const std::stop_token&,
            const gc::NodeProgress& progress) const
        -> bool override
    {
        assert(inputs.size() == 7_gc_ic);
        assert(result.size() == 1_gc_oc);
        const auto& size = inputs[0_gc_i].as<UintSize>();
        auto lowest_state = inputs[1_gc_i].convert_to<int8_t>();
        auto range_size = inputs[2_gc_i].convert_to<int8_t>();
        const auto& map = inputs[3_gc_i].as<std::vector<int8_t>>();
        auto radius = inputs[4_gc_i].convert_to<int>();
        auto shape = inputs[5_gc_i].convert_to<std::string_view>();
        auto outer_state = inputs[6_gc_i].convert_to<int8_t>();

        result[0_gc_o] = generate_image(
            size, lowest_state, range_size, map, radius, shape, outer_state);

        if (progress)
            progress(1);

        return true;
    }

private:
    static auto generate_image(
        const UintSize& size,
        int8_t lowest_state,
        int8_t range_size,
        const std::vector<int8_t>& map,
        int radius,
        std::string_view shape_str,
        int8_t outer_state) -> I8Image
    {
        if (range_size == 0)
            throw std::invalid_argument(
                "RandomImage: range_size must be positive");
        if (size.width < 1 || size.height < 1)
            throw std::invalid_argument(
                "RandomImage: image width and height must both be positive");

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int8_t> distrib(
            lowest_state, lowest_state + range_size - 1);

        auto image = I8Image{
            .size = size,
            .data = std::vector<int8_t>(size.width * size.height, 0)
        };
        if (radius < 0)
            for (auto& pixel : image.data)
                pixel = distrib(gen);
        else
        {
            enum class Shape{ circle, rectangle };
            auto shape = [shape_str]{
                if (shape_str == "circle")
                    return Shape::circle;
                if (shape_str == "rectangle")
                    return Shape::rectangle;
                common::throw_<std::invalid_argument>(
                    "Invalid shape '", shape_str, "'");
            }();

            int width = size.width;
            int height = size.height;
            int xc = width / 2;
            int yc = height / 2;
            auto* pixel = image.data.data();
            auto r2 = radius * radius;
            for (int y=0; y<height; ++y)
                for (int x=0; x<width; ++x, ++pixel)
                {
                    auto dx = x - xc;
                    auto dy = y - yc;
                    switch(shape)
                    {
                    case Shape::rectangle:
                        if (std::abs(dx) > radius || std::abs(dy) > radius)
                        {
                            *pixel = outer_state;
                            continue;
                        }
                        break;
                    case Shape::circle:
                        if (dx*dx + dy*dy > r2)
                        {
                            *pixel = outer_state;
                            continue;
                        }
                    }
                    *pixel = distrib(gen);
                }
        }

        if( !map.empty() )
        {
            if (map.size() != static_cast<size_t>(range_size))
                throw std::invalid_argument(
                    "RandomImage: Map size must be either zero or range_size");

            // Do the mapping
            for (auto& pixel : image.data)
                pixel = map.at(pixel - lowest_state) + lowest_state;
        }

        return image;
    }

};

auto make_random_image(gc::ConstValueSpan args, const gc::ComputationContext&)
    -> std::shared_ptr<gc::ComputationNode>
{
    gc::expect_no_node_args("RandomImage", args);
    return std::make_shared<RandomImage>();
}

} // namespace gc_app::cell_aut
