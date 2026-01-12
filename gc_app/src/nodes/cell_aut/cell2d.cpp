/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#include "gc_app/nodes/cell_aut/cell2d.hpp"

#include "gc_app/types/image.hpp"
#include "gc_app/types/cell2d_rules.hpp"

#include "gc/expect_n_node_args.hpp"
#include "gc/computation_node.hpp"
#include "gc/node_port_names.hpp"

#include "common/func_ref.hpp"

#include <cassert>


using namespace std::string_view_literals;
using namespace gc::literals;

namespace gc_app::cell_aut {

namespace {

constexpr int8_t NoChange = -128;

template <int NeighborhoodSize>
class RtRules final
{
public:
    RtRules(
        uint8_t state_count,
        int8_t min_state,
        const std::vector<int8_t>& m):
    state_count_{state_count},
    min_state_{min_state},
    min_sum_{min_state*NeighborhoodSize},
    max_sum_{min_sum_ + (state_count_-1)*NeighborhoodSize},
    m_{m.data() - min_sum_}
    {}

    auto operator()(int8_t value, int16_t sum) const -> int8_t
    {
        if (sum < min_sum_ || sum > max_sum_)
            throw std::out_of_range("Sum of cell values is out of range");
        auto mapped = m_[sum];
        return mapped == NoChange? value: mapped;
    }

private:
    uint8_t state_count_;
    int8_t min_state_;
    int min_sum_;
    int max_sum_;
    const int8_t* m_;
};

} // anonymous namespace


class Cell2d final :
    public gc::ComputationNode
{
public:
    auto input_names() const
        -> gc::InputNames override
    { return gc::node_input_names<Cell2d>( "rules"sv, "input_state"sv ); }

    auto output_names() const
        -> gc::OutputNames override
    { return gc::node_output_names<Cell2d>( "output_state"sv ); }

    auto default_inputs(gc::InputValues result) const
        -> void override
    {
        assert(result.size() == 2_gc_ic);

        result[0_gc_i] = Cell2dRules{};

        constexpr Uint w = 100;
        constexpr Uint h = 100;
        result[1_gc_i] = I8Image
        {
            .size = {w, h},
            .data = std::vector<int8_t>(w*h, 0)
        };
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
        const auto& rules = inputs[0_gc_i].as<Cell2dRules>();
        const auto& in_image = inputs[1_gc_i].as<I8Image>();

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

        advance(out_image, in_image, rules);

        if (progress)
            progress(1);
        return true;
    }

private:
    static constexpr struct Tor_Tag final {} Tor{};
    static constexpr struct Rect_Tag final {} Rect{};

    template <bool count_self>
    static auto advance_impl(
        Tor_Tag,
        std::bool_constant<count_self>,
        I8Image& out,
        const I8Image& in,
        const Cell2dRules& rules) -> void
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

        auto n_sum = [](auto const* prev, auto const* cur, auto const* next)
            -> int16_t
        {
            if constexpr(count_self)
                return
                    prev[0] + prev[1] + prev[2] +
                     cur[0] +  cur[1] +  cur[2] +
                    next[0] + next[1] + next[2];
            else
                return
                    prev[0] + prev[1] + prev[2] +
                     cur[0] +            cur[2] +
                    next[0] + next[1] + next[2];
        };

        auto n_sum_l = [&](auto const* prev, auto const* cur, auto const* next)
            -> int16_t
        {
            if constexpr(count_self)
                return
                    prev[w-1] + prev[0] + prev[1] +
                     cur[w-1] +  cur[0] +  cur[1] +
                    next[w-1] + next[0] + next[1];
            else
                return
                    prev[w-1] + prev[0] + prev[1] +
                     cur[w-1] +            cur[1] +
                    next[w-1] + next[0] + next[1];
        };

        auto n_sum_r = [w](auto const* prev, auto const* cur, auto const* next)
            -> int16_t
        {
            if constexpr(count_self)
                return
                    prev[w-2] + prev[w-1] + prev[0] +
                     cur[w-2] +  cur[w-1] +  cur[0] +
                    next[w-2] + next[w-1] + next[0];
            else
                return
                    prev[w-2] + prev[w-1] + prev[0] +
                     cur[w-2] +              cur[0] +
                    next[w-2] + next[w-1] + next[0];
        };

        auto rtr9 = RtRules<9>(
            rules.state_count, rules.min_state, rules.map9);

        auto const* prev_line = line(src, h-1);
        auto const* cur_line = line(src, 0);
        for (size_t y=0; y<h; ++y)
        {
            auto const* next_line = line(src, (y+1)%h);
            auto* dst_line = line(dst, y);
            dst_line[0] =
                rtr9(cur_line[0], n_sum_l(prev_line, cur_line, next_line));
            for (size_t x=0; x+2<w; ++x)
            {
                auto s = n_sum(prev_line+x, cur_line+x, next_line+x);
                dst_line[x+1] = rtr9(cur_line[x+1], s);
            }
            dst_line[w-1] =
                rtr9(cur_line[w-1], n_sum_r(prev_line, cur_line, next_line));
            prev_line = cur_line;
            cur_line = next_line;
        }
    }

    template <bool count_self>
    static auto advance_impl(
        Rect_Tag,
        std::bool_constant<count_self>,
        I8Image& out,
        const I8Image& in,
        const Cell2dRules& rules) -> void
    {
        assert(out.size == in.size);
        auto h = in.size.height;
        auto w = in.size.width;
        if (h < 2 || w < 2)
            return;
        auto const* src = in.data.data();
        auto* dst = out.data.data();

        auto line = [&](auto* cells, size_t y)
        {
            return cells + y*w;
        };

        auto n_sum = [](auto const* prev, auto const* cur, auto const* next)
            -> int16_t
        {
            if constexpr(count_self)
                return
                    prev[-1] + prev[0] + prev[1] +
                     cur[-1] +  cur[0] +  cur[1] +
                    next[-1] + next[0] + next[1];
            else
                return
                    prev[-1] + prev[0] + prev[1] +
                     cur[-1] +            cur[1] +
                    next[-1] + next[0] + next[1];
        };

        auto n_sum_t = [&](auto const* cur, auto const* next)
            -> int16_t
        {
            if constexpr(count_self)
                return
                     cur[-1] +  cur[0] +  cur[1] +
                    next[-1] + next[0] + next[1];
            else
                return
                     cur[-1] +            cur[1] +
                    next[-1] + next[0] + next[1];
        };

        auto n_sum_b = [&](auto const* prev, auto const* cur)
            -> int16_t
        {
            if constexpr(count_self)
                return
                    prev[-1] + prev[0] + prev[1] +
                     cur[-1] +  cur[0] +  cur[1];
            else
                return
                    prev[-1] + prev[0] + prev[1] +
                     cur[-1] +            cur[1];
        };

        auto n_sum_l = [&](auto const* prev, auto const* cur, auto const* next)
            -> int16_t
        {
            if constexpr(count_self)
                return
                    prev[0] + prev[1] +
                     cur[0] +  cur[1] +
                    next[0] + next[1];
            else
                return
                    prev[0] + prev[1] +
                               cur[1] +
                    next[0] + next[1];
        };

        auto n_sum_lt = [&](auto const* cur, auto const* next)
            -> int16_t
        {
            if constexpr(count_self)
                return
                     cur[0] + cur[1] +
                    next[0] + next[1];
            else
                return
                               cur[1] +
                    next[0] + next[1];
        };

        auto n_sum_lb = [&](auto const* prev, auto const* cur)
            -> int16_t
        {
            if constexpr(count_self)
                return
                    prev[0] + prev[1] +
                     cur[0] +  cur[1];
            else
                return
                    prev[0] + prev[1] +
                               cur[1];
        };

        auto n_sum_r = [w](auto const* prev, auto const* cur, auto const* next)
            -> int16_t
        {
            if constexpr(count_self)
                return
                    prev[w-2] + prev[w-1] +
                     cur[w-2] +  cur[w-1] +
                    next[w-2] + next[w-1];
            else
                return
                    prev[w-2] + prev[w-1] +
                     cur[w-2] +           +
                    next[w-2] + next[w-1];
        };

        auto n_sum_rt = [&](auto const* cur, auto const* next)
            -> int16_t
        {
            if constexpr(count_self)
                return
                     cur[w-2] +  cur[w-1] +
                    next[w-2] + next[w-1];
            else
                return
                     cur[w-2] +           +
                    next[w-2] + next[w-1];
        };

        auto n_sum_rb = [&](auto const* prev, auto const* cur)
            -> int16_t
        {
            if constexpr(count_self)
                return
                    prev[w-2] + prev[w-1] +
                     cur[w-2] +  cur[w-1];
            else
                return
                    prev[w-2] + prev[w-1] +
                     cur[w-2];
        };

        auto rtr9 = RtRules<9>(
            rules.state_count, rules.min_state, rules.map9);
        auto rtr6 = RtRules<6>(
            rules.state_count, rules.min_state, rules.map6);
        auto rtr4 = RtRules<4>(
            rules.state_count, rules.min_state, rules.map4);

        auto const* prev_line = line(src, 0);
        auto const* cur_line = prev_line;
        {
            auto const* next_line = line(src, 1);
            auto* dst_line = line(dst, 0);
            dst_line[0] =
                rtr4(cur_line[0], n_sum_lt(cur_line, next_line));
            for (size_t x=1; x+1<w; ++x)
            {
                dst_line[x] =
                    rtr6(cur_line[x], n_sum_t(cur_line+x, next_line+x));
            }
            dst_line[w-1] =
                rtr4(cur_line[w-1], n_sum_rt(cur_line, next_line));
            prev_line = cur_line;
            cur_line = next_line;
        }
        for (size_t y=1; y+1<h; ++y)
        {
            auto const* next_line = line(src, (y+1));
            auto* dst_line = line(dst, y);
            dst_line[0] =
                rtr6(cur_line[0], n_sum_l(prev_line, cur_line, next_line));
            for (size_t x=1; x+1<w; ++x)
            {
                dst_line[x] = rtr9(
                    cur_line[x],
                    n_sum(prev_line+x, cur_line+x, next_line+x));
            }
            dst_line[w-1] =
                rtr6(cur_line[w-1], n_sum_r(prev_line, cur_line, next_line));
            prev_line = cur_line;
            cur_line = next_line;
        }
        {
            auto* dst_line = line(dst, h-1);
            dst_line[0] =
                rtr4(cur_line[0], n_sum_lb(prev_line, cur_line));
            for (size_t x=1; x+1<w; ++x)
            {
                dst_line[x] =
                    rtr6(cur_line[x], n_sum_b(prev_line+x, cur_line+x));
            }
            dst_line[w-1] =
                rtr4(cur_line[w-1], n_sum_rb(prev_line, cur_line));
        }
    }

    static auto advance(
        I8Image& out, const I8Image& in, const Cell2dRules& rules) -> void
    {
        auto on_count_self = [&](auto shape, auto count_self)
        {
            advance_impl(shape, count_self, out, in, rules);
        };

        auto on_shape = [&](auto shape)
        {
            if (rules.count_self)
                on_count_self(shape, std::true_type{});
            else
                on_count_self(shape, std::false_type{});
        };

        if (rules.tor)
            on_shape(Tor);
        else
            on_shape(Rect);
    }
};

auto make_cell2d(gc::ConstValueSpan args, const gc::ComputationContext&)
    -> std::shared_ptr<gc::ComputationNode>
{
    gc::expect_no_node_args("Cell2d", args);
    return std::make_shared<Cell2d>();
}

} // namespace gc_app::cell_aut
