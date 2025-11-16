/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#include "gc_app/nodes/cell_aut/generate_rules.hpp"

#include "gc_app/types/cell2d_gen_rules.hpp"
#include "gc_app/types/cell2d_rules.hpp"

#include "gc/expect_n_node_args.hpp"
#include "gc/computation_node.hpp"
#include "gc/node_port_names.hpp"
#include "gc/value.hpp"

#include "common/expr_calculator.hpp"
#include "common/func_ref.hpp"

#include <cassert>
#include <cstring>
#include <fstream>


using namespace std::literals;
using namespace gc::literals;

namespace gc_app::cell_aut {

namespace {

constexpr int8_t NoChange = -128;

template <typename T>
struct I
{
    explicit I(T& v) noexcept : v{v} {}
    T& v;
};

template <typename T>
auto operator>>(std::istream& s, I<T>&& i) -> std::istream&
{
    int v;
    s >> v;
    i.v = static_cast<T>(v);
    return s;
}

auto rtrim(std::string& s) -> void
{
    if (s.empty())
        return;
    int pos=s.size() - 1;
    for (; pos>=0; --pos)
        if (!strchr(" \r\n\t", s[pos]))
            break;
    s.erase(pos+1);
}

auto generate_rules(const Cell2dGenRules& gen_rules) -> Cell2dRules
{
    auto fill_map = [&](
        std::vector<int8_t>& map,
        const Cell2dGenRules::Overlay& overlay,
        int nbrs,
        const std::string& context)
    {
        try {
            auto calc = common::ExprCalculator{overlay.formula};
            auto variables = std::unordered_map<std::string_view, double>{};
            auto& n_var = variables["n"sv];
            auto offset = gen_rules.min_state * nbrs;
            auto step = std::min(overlay.step, 1);
            for(int n=overlay.min_sum; n<=overlay.max_sum; n+=step)
            {
                n_var = n;
                map.at(n-offset) = calc(variables);
            }
        }
        catch(std::exception& e) {
            common::throw_("generate_rules: ", context, ": ", e.what());
        }
    };

    auto test_map = [&](
        std::vector<int8_t>& map,
        int nbrs,
        const std::string& context)
    {
        int8_t min = gen_rules.min_state;
        int8_t max = gen_rules.min_state + (gen_rules.state_count - 1);

        for(auto i : common::index_range<size_t>(map.size()))
        {
            if((map[i] > max || map[i] < min) && map[i] != NoChange)
            {
                common::throw_(
                    "generate_rules: ", context, ": Map element ", i,
                    " is out of range (==", int(map[i]), ")");
            }
        }
    };

    auto generate_map = [&](
        const Cell2dGenRules::Map& gen_map,
        int nbrs,
        const std::string& context)
    {
        auto map_length = (gen_rules.state_count - 1) * nbrs + 1;
        auto min_sum = gen_rules.min_state * nbrs;
        auto max_sum = min_sum + map_length - 1;

        auto result = std::vector<int8_t>(map_length, 0);

        fill_map(
            result,
            {
                .formula = gen_map.formula,
                .min_sum = min_sum,
                .max_sum = max_sum,
                .step = 1
            },
            nbrs,
            context + ", main formula");

        for (size_t overlay_index=0; auto& overlay : gen_map.overlays)
            fill_map(result,
                     overlay,
                     nbrs,
                     common::format(context, ", overlay ", overlay_index++));

        test_map(result, nbrs, context);

        return result;
    };

    return
    {
        .state_count = gen_rules.state_count,
        .min_state = gen_rules.min_state,
        .tor = gen_rules.tor,
        .count_self = gen_rules.count_self,
        .map9 = generate_map(gen_rules.map9, 9, "map9"),
        .map6 = generate_map(gen_rules.map9, 6, "map6"),
        .map4 = generate_map(gen_rules.map9, 4, "map4")
    };
}

} // anonymous namespace

class GenerateRules final :
    public gc::ComputationNode
{
public:
    auto input_names() const
        -> gc::InputNames override
    {
        return gc::node_input_names<GenerateRules>("gen_rules"sv);
    }

    auto output_names() const
        -> gc::OutputNames override
    { return gc::node_output_names<GenerateRules>("rules"sv); }

    auto default_inputs(gc::InputValues result) const
        -> void override
    {
        assert(result.size() == 1_gc_ic);
        result[0_gc_i] = Cell2dGenRules{};
    }

    auto compute_outputs(
            gc::OutputValues result,
            gc::ConstInputValues inputs,
            const std::stop_token& stoken,
            const gc::NodeProgress& progress) const
        -> bool override
    {
        assert(inputs.size() == 1_gc_ic);
        assert(result.size() == 1_gc_oc);
        const auto& gen_rules = inputs[0_gc_i].as<Cell2dGenRules>();
        result[0_gc_o] = generate_rules(gen_rules);

        if (progress)
            progress(1);

        return true;
    }
};

auto make_generate_rules(gc::ConstValueSpan args, const gc::ComputationContext&)
    -> std::shared_ptr<gc::ComputationNode>
{
    gc::expect_no_node_args("GenerateRules", args);
    return std::make_shared<GenerateRules>();
}

} // namespace gc_app::cell_aut
