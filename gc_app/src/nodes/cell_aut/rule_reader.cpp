/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#include "gc_app/nodes/cell_aut/rule_reader.hpp"

#include "gc_app/types/cell2d_rules.hpp"

#include "gc/expect_n_node_args.hpp"
#include "gc/computation_node.hpp"
#include "gc/node_port_names.hpp"
#include "gc/value.hpp"

#include "common/func_ref.hpp"

#include <cassert>
#include <cstring>
#include <fstream>


using namespace std::literals;
using namespace gc::literals;

namespace gc_app::cell_aut {

namespace {

constexpr int8_t NoChange = -128;

void ignore_comments(std::istream& f)
{
    auto s = std::string{};
    auto c = char{};

    while(f.get(c), c == '%')
        getline(f, s);
    f.unget();
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

auto read_rules(const std::string& path) -> Cell2dRules
{
    auto f = std::fstream( path.c_str() );
    if( !f.is_open() )
        throw std::runtime_error(
            common::format("read_rules: Can't open rules file ", path));
    f.exceptions(std::ios::failbit);
    ignore_comments(f);
    auto rules = Cell2dRules{};
    f >> I(rules.state_count) >> I(rules.min_state);
    if(rules.state_count < 1)
        throw std::invalid_argument("read_rules: Invalid state count");
    rules.map9 = std::vector<int8_t>((rules.state_count-1)*9+1, 0);
    rules.map6 = std::vector<int8_t>((rules.state_count-1)*6+1, 0);
    rules.map4 = std::vector<int8_t>((rules.state_count-1)*4+1, 0);

    auto read_rule = [&](std::string_view rule_type, size_t pos)
    {
        int8_t v;
        f >> I(v);
        if(( v < rules.min_state || v >= rules.min_state + rules.state_count) &&
            v != NoChange )
        {
            throw std::invalid_argument(
                common::format(rule_type, " is out of range, pos. ", pos));
        }
        return v;
    };

    for(size_t i=0; auto& rule : rules.map9)
        rule = read_rule("map9", i++);

    auto buf = std::string{};
    getline(f, buf);
    getline(f, buf);
    rtrim(buf);
    if(buf == "END")
        return rules;

    if(buf != "BOUNDS")
        throw std::invalid_argument( "Invalid boundary rules" );

    for(size_t i=0; auto& rule : rules.map6)
        rule = read_rule("map6", i++);

    for(size_t i=0; auto& rule : rules.map4)
        rule = read_rule("map4", i++);

    rules.tor = false;

    // NB: Cell2dRules::count_self is historically NOT in the rules file

    return rules;
}

} // anonymous namespace

class RuleReader final :
    public gc::ComputationNode
{
public:
    auto input_names() const
        -> gc::InputNames override
    {
        return gc::node_input_names<RuleReader>("file"sv);
    }

    auto output_names() const
        -> gc::OutputNames override
    { return gc::node_output_names<RuleReader>("rules"sv); }

    auto default_inputs(gc::InputValues result) const
        -> void override
    {
        assert(result.size() == 1_gc_ic);
        result[0_gc_i] = "life.rul"s;
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
        const auto& path = inputs[0_gc_i].as<std::string>();
        result[0_gc_o] = read_rules(path);

        if (progress)
            progress(1);

        return true;
    }
};

auto make_rule_reader(gc::ConstValueSpan args, const gc::ComputationContext&)
    -> std::shared_ptr<gc::ComputationNode>
{
    gc::expect_no_node_args("RuleReader", args);
    return std::make_shared<RuleReader>();
}

} // namespace gc_app::cell_aut
