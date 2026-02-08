/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025-2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#include "gc_app/nodes/cell_aut/life.hpp"

#include "gc_types/image.hpp"
#include "gc_types/uint.hpp"

#include "gc/expect_n_node_args.hpp"
#include "gc/computation_node.hpp"
#include "gc/node_port_names.hpp"

#include "common/func_ref.hpp"

#include <cassert>


using namespace std::string_view_literals;
using namespace gc::literals;

namespace gc_app::cell_aut {

using namespace gc_types;

class Life final :
    public gc::ComputationNode
{
public:
    auto input_names() const
        -> gc::InputNames override
    { return gc::node_input_names<Life>( "input"sv ); }

    auto output_names() const
        -> gc::OutputNames override
    { return gc::node_output_names<Life>( "output"sv ); }

    auto default_inputs(gc::InputValues result) const
        -> void override
    {
        assert(result.size() == 1_gc_ic);
        constexpr Uint w = 100;
        constexpr Uint h = 100;
        constexpr Uint m = w/2;
        auto c = std::vector<int8_t>(w*h, 0);


        // Glider
        // c[w*m+m] = 1;
        // c[w*m+m+1] = 1;
        // c[w*m+m-1] = 1;
        // c[w*(m+1)+m+1] = 1;
        // c[w*(m+2)+m] = 1;

        // Acorn
        c[w*m+m] = 1;
        c[w*m+m+1] = 1;
        c[w*m+m+4] = 1;
        c[w*m+m+5] = 1;
        c[w*m+m+6] = 1;
        c[w*(m+1)+m+3] = 1;
        c[w*(m+2)+m+1] = 1;

        result[0_gc_i] = I8Image
        {
            .size = {w, h},
            .data = std::move(c)
        };
    }

    auto compute_outputs(
            gc::OutputValues result,
            gc::ConstInputValues inputs,
            const std::stop_token&,
            const gc::NodeProgress& progress) const
        -> bool override
    {
        assert(inputs.size() == 1_gc_ic);
        assert(result.size() == 1_gc_oc);
        auto const& in_image = inputs.front().as<I8Image>();

        auto& out_image = [&]() -> I8Image&
        {
            static const auto* I8Image_type = gc::type_of<I8Image>();
            auto& out = result.front();
            if (out.type() == I8Image_type)
            {
                auto& image = out.as<I8Image>();
                if (image.size == in_image.size)
                    return image;
            }
            out = in_image;
            return out.as<I8Image>();
        }();

        advance(out_image, in_image);

        if (progress)
            progress(1);
        return true;
    }

private:
    static auto advance(I8Image& out, I8Image const& in) -> void
    {
        assert(out.size == in.size);
        auto h = in.size.height;
        auto w = in.size.width;
        if (h == 0 || w == 0)
            return;
        auto const* src = in.data.data();
        auto* dst = out.data.data();

        auto line = [&](auto* cells, size_t y)
        {
            return cells + y*w;
        };

        auto n_sum_general = [](auto const* prev, auto const* cur, auto const* next)
        {
            return
                prev[0] + prev[1] + prev[2] +
                cur[0] +            cur[2] +
                next[0] + next[1] + next[2];
        };

        auto n_sum_left = [w](auto const* prev, auto const* cur, auto const* next)
        {
            return
                prev[w-1] + prev[0] + prev[1] +
                cur[w-1] +            cur[1] +
                next[w-1] + next[0] + next[1];
        };

        auto n_sum_right = [w](auto const* prev, auto const* cur, auto const* next)
        {
            return
                prev[w-2] + prev[w-1] + prev[0] +
                cur[w-2] +              cur[0] +
                next[w-2] + next[w-1] + next[0];
        };

        constexpr uint8_t rules[2][9] =
            {{ 0, 0, 0, 1, 0, 0, 0, 0, 0 },
             { 0, 0, 1, 1, 0, 0, 0, 0, 0 }};

        auto const* prev_line = line(src, h-1);
        auto const* cur_line = line(src, 0);
        for (size_t y=0; y<h; ++y)
        {
            auto const* next_line = line(src, (y+1)%h);
            auto* dst_line = line(dst, y);
            dst_line[0] =
                rules[cur_line[0]][n_sum_left(prev_line, cur_line, next_line)];
            for (size_t x=0; x+2<w; ++x)
            {
                auto s = n_sum_general(prev_line+x, cur_line+x, next_line+x);
                dst_line[x+1] = rules[cur_line[x+1]][s];
            }
            dst_line[w-1] =
                rules[cur_line[w-1]]
                    [n_sum_right(prev_line, cur_line, next_line)];
            prev_line = cur_line;
            cur_line = next_line;
        }
    }
};

auto make_life(gc::ConstValueSpan args, const gc::ComputationContext&)
    -> std::shared_ptr<gc::ComputationNode>
{
    gc::expect_no_node_args("Life", args);
    return std::make_shared<Life>();
}

} // namespace gc_app::cell_aut
