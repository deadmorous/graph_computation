/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#include "gc_app/nodes/cell_aut/gen_rule_reader.hpp"

#include "gc_app/types/cell2d_gen_rules.hpp"

#include "gc/expect_n_node_args.hpp"
#include "gc/computation_node.hpp"
#include "gc/node_port_names.hpp"
#include "gc/value.hpp"

#include "common/expr_calculator.hpp"
#include "common/func_ref.hpp"

#include <algorithm>
#include <cctype> // For std::tolower/std::toupper
#include <cassert>
#include <cstring>
#include <fstream>


using namespace std::literals;
using namespace gc::literals;

namespace gc_app::cell_aut {

namespace {

/* ==== begin vibe-code (Google Gemini) ====
 * Prompt:
 * In a C++ program, I need to do a case-insensitive search in an std::string.
 * What is the best recommended way to do so without relying on any third-party
 * libraries?
 */

/**
 * Custom binary predicate for case-insensitive character comparison.
 * Compares two characters after converting them to lowercase.
 */
bool case_insensitive_char_comp(char a, char b) {
    // Note: It's generally safer to cast to unsigned char before passing
    // to tolower/toupper to avoid issues with negative char values.
    return std::tolower(static_cast<unsigned char>(a)) ==
           std::tolower(static_cast<unsigned char>(b));
}

/**
 * Performs a case-insensitive search for a substring (needle)
 * within a main string (haystack).
 *
 * @returns An iterator to the first character of the found substring,
 * or haystack.end() if not found.
 */
std::string::iterator case_insensitive_search(
    std::string& haystack,
    const std::string& needle)
{
    return std::search(
        haystack.begin(),
        haystack.end(),
        needle.begin(),
        needle.end(),
        case_insensitive_char_comp
        );
}

// ==== end vibe-code ====

bool ignore_comments_and_check_for_count_center_cell(std::istream& f)
{
    auto s = std::string{};
    auto c = char{};

    bool result = false;

    while(f.get(c), c == '%')
    {
        getline(f, s);
        if (!result)
            result = case_insensitive_search(s, "count center cell") != s.end();
    }

    f.unget();
    return result;
}

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

auto read_gen_rules(const std::string& path) -> Cell2dGenRules
{
    auto f = std::fstream( path.c_str() );
    if( !f.is_open() )
        throw std::runtime_error(
            common::format(
                "read_gen_rules: Can't open rule generation file ", path));
    f.exceptions(std::ios::failbit);
    auto count_center_cell = ignore_comments_and_check_for_count_center_cell(f);
    auto gen_rules = Cell2dGenRules{
        .count_self = count_center_cell
    };

    auto read_rtrimmed_line = [&]{
        auto result = std::string{};
        getline(f, result);
        rtrim(result);
        return result;
    };

    f >> I(gen_rules.state_count) >> I(gen_rules.min_state);
    if(gen_rules.state_count < 1   ||   gen_rules.state_count > 255)
        throw std::invalid_argument("read_gen_rules: Invalid state count");
    read_rtrimmed_line();   // Ignore EOL

    auto read_checked_formula = [&](const std::string& context)
    {
        try {
            auto formula = read_rtrimmed_line();
            auto expr = common::ExprCalculator{formula};
            expr({{"n", 0.}});
            return formula;
        }
        catch (std::exception& e) {
            common::throw_(
                "read_gen_rules: ", context,
                ": failed to read formula: ", e.what());
        }
    };

    auto read_gen_rule = [&](int nbrs, const std::string& context)
    {
        auto map_length = (gen_rules.state_count - 1) * nbrs + 1;
        auto min_sum = gen_rules.min_state * nbrs;
        auto max_sum = min_sum + map_length - 1;

        auto result = Cell2dGenRules::Map{};
        result.formula = read_checked_formula(context);

        auto overlay_count = size_t{};
        f >> overlay_count;
        read_rtrimmed_line();   // Ignore EOL

        for (size_t overlay_index : common::index_range<size_t>(overlay_count))
        {
            auto overlay_context =
                common::format(context, ", overlay ", overlay_index);
            auto overlay = Cell2dGenRules::Overlay{};
            f >> overlay.range.min >> overlay.range.max >> overlay.range.step;
            if( overlay.range.max == -1000000 )
                overlay.range.max = map_length - 1;
            if( overlay.range.min  == -1000000 )
                overlay.range.min  = map_length - 1;
            if (!overlay.range.ok(min_sum, max_sum))
            {
                common::throw_(
                    "read_gen_rules: ", overlay_context,
                    ": Invalid range: ", gc::Value{overlay.range});
            }

            read_rtrimmed_line();   // Ignore EOL
            overlay.formula = read_checked_formula(overlay_context);
            result.overlays.push_back(std::move(overlay));
        }
        return result;
    };

    gen_rules.map9 = read_gen_rule(9, "map9");

    auto line = read_rtrimmed_line();
    if(line == "END")
        return gen_rules;

    if(line != "BOUNDS")
        throw std::invalid_argument("Invalid boundary rules");

    gen_rules.map6 = read_gen_rule(6, "map6");

    gen_rules.map4 = read_gen_rule(4, "map4");

    gen_rules.tor = false;

    // NB: Cell2dRules::count_self is historically NOT in the rules file

    return gen_rules;
}

} // anonymous namespace

class GenRuleReader final :
    public gc::ComputationNode
{
public:
    auto input_names() const
        -> gc::InputNames override
    {
        return gc::node_input_names<GenRuleReader>("file"sv);
    }

    auto output_names() const
        -> gc::OutputNames override
    { return gc::node_output_names<GenRuleReader>("gen_rules"sv); }

    auto default_inputs(gc::InputValues result) const
        -> void override
    {
        assert(result.size() == 1_gc_ic);
        result[0_gc_i] = "hunt.gen"s;
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
        const auto& path = inputs[0_gc_i].as<std::string>();
        result[0_gc_o] = read_gen_rules(path);

        if (progress)
            progress(1);

        return true;
    }
};

auto make_gen_rule_reader(gc::ConstValueSpan args, const gc::ComputationContext&)
    -> std::shared_ptr<gc::ComputationNode>
{
    gc::expect_no_node_args("GenRuleReader", args);
    return std::make_shared<GenRuleReader>();
}

} // namespace gc_app::cell_aut
