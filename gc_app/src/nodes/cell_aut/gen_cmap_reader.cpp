/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#include "gc_app/nodes/cell_aut/gen_cmap_reader.hpp"

#include "gc_app/types/cell2d_gen_cmap.hpp"

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

auto read_gen_cmap(const std::string& path) -> Cell2dGenCmap
{
    auto f = std::fstream( path.c_str() );
    if( !f.is_open() )
        throw std::runtime_error(
            common::format(
                "read_gen_cmap: Can't open color map generation file ", path));
    f.exceptions(std::ios::failbit);
    ignore_comments(f);
    auto gen_cmap = Cell2dGenCmap{};

    auto read_rtrimmed_line = [&]{
        auto result = std::string{};
        getline(f, result);
        rtrim(result);
        return result;
    };

    f >> gen_cmap.state_count;
    if(gen_cmap.state_count < 1   ||   gen_cmap.state_count > 256)
        throw std::invalid_argument("read_gen_cmap: Invalid state count");
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
                "read_gen_cmap: ", context,
                ": failed to read formula: ", e.what());
        }
    };

    gen_cmap.formula.r = read_checked_formula("main formula, red");
    gen_cmap.formula.g = read_checked_formula("main formula, green");
    gen_cmap.formula.b = read_checked_formula("main formula, blue");

    auto overlay_count = size_t{};
    f >> overlay_count;
    read_rtrimmed_line();   // Ignore EOL

    auto max = gen_cmap.state_count - 1;

    for (size_t overlay_index : common::index_range<size_t>(overlay_count))
    {
        auto overlay_context =
            common::format("overlay ", overlay_index);
        auto overlay = Cell2dGenCmap::Overlay{};
        f >> overlay.range.min >> overlay.range.max >> overlay.range.step;
        if(overlay.range.max < 0)
            overlay.range.max = max;
        if(overlay.range.min < 0)
            overlay.range.min = max;
        if (!overlay.range.ok(0, max))
        {
            common::throw_(
                "read_gen_cmap: ", overlay_context,
                ": Invalid range: ", gc::Value{overlay.range});
        }

        read_rtrimmed_line();   // Ignore EOL
        overlay.formula.r = read_checked_formula(overlay_context + ", red");
        overlay.formula.g = read_checked_formula(overlay_context + ", green");
        overlay.formula.b = read_checked_formula(overlay_context + ", blue");
        gen_cmap.overlays.push_back(std::move(overlay));
    }
    return gen_cmap;
}

} // anonymous namespace

class GenCmapReader final :
    public gc::ComputationNode
{
public:
    auto input_names() const
        -> gc::InputNames override
    {
        return gc::node_input_names<GenCmapReader>("file"sv);
    }

    auto output_names() const
        -> gc::OutputNames override
    { return gc::node_output_names<GenCmapReader>("gen_cmap"sv); }

    auto default_inputs(gc::InputValues result) const
        -> void override
    {
        assert(result.size() == 1_gc_ic);
        result[0_gc_i] = "bw.cf"s;
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
        result[0_gc_o] = read_gen_cmap(path);

        if (progress)
            progress(1);

        return true;
    }
};

auto make_gen_cmap_reader(gc::ConstValueSpan args, const gc::ComputationContext&)
    -> std::shared_ptr<gc::ComputationNode>
{
    gc::expect_no_node_args("GenCmapReader", args);
    return std::make_shared<GenCmapReader>();
}

} // namespace gc_app::cell_aut
