#include "gc/graph_computation.hpp"
#include "gc/node.hpp"
#include "gc/node_port_names.hpp"

#include "common/format.hpp"

#include <gtest/gtest.h>

#include <initializer_list>
#include <numeric>
#include <ranges>


using namespace std::literals;

namespace {

class TestNode final
    : public gc::Node
{
public:
    TestNode(uint32_t input_count,
             uint32_t output_count)
    {
        input_names_.resize(input_count);
        output_names_.resize(output_count);
    }

    auto input_names() const
        -> common::ConstNameSpan override
    { return input_names_(); }

    auto output_names() const
        -> common::ConstNameSpan override
    { return output_names_(); }

    auto default_inputs(gc::ValueSpan result) const
        -> void override
    { std::fill(result.begin(), result.end(), 0); }

    auto compute_outputs(gc::ValueSpan result,
                         gc::ConstValueSpan inputs,
                         const std::stop_token& stoken,
                         const gc::NodeProgress& progress) const
        -> bool override
    {
        ++computation_count_;

        std::iota(result.begin(), result.end(), 1);

        if (output_count() == 0)
            return true;

        auto output_index = 0;
        for (const auto& input : inputs)
        {
            result[output_index].as<int>() += input.as<int>();
            output_index = (output_index + 1) % output_count();
        }

        return true;
    }

    auto computation_count() const noexcept
        -> size_t
    { return computation_count_; }

private:
    mutable size_t computation_count_{};
    gc::DynamicInputNames input_names_;
    gc::DynamicOutputNames output_names_;
};

struct TestGraphNodeSpec
{
    uint32_t input_count{};
    uint32_t output_count{};
};

auto test_graph(std::initializer_list<TestGraphNodeSpec> nodes,
                std::initializer_list<gc::Edge> edges)
    -> gc::Graph
{
    auto result = gc::Graph{};
    for (const auto& node_spec : nodes)
        result.nodes.emplace_back(
            std::make_shared<TestNode>(node_spec.input_count,
                                       node_spec.output_count));

    result.edges.reserve(edges.size());
    std::copy(edges.begin(), edges.end(), back_inserter(result.edges));

    return result;
}

auto check_comple_graph(const gc::Graph& g,
                        std::string_view expected_formatted_instructions,
                        const gc::SourceInputs& expected_source_inputs = {})
    -> void
{

    auto [instructions, source_inputs] = compile(g);

    // std::cout << "Compiled instructions: " << *instructions << std::endl;

    EXPECT_EQ(common::format(*instructions), expected_formatted_instructions);
    EXPECT_EQ(source_inputs, expected_source_inputs);
}

auto edge(std::pair<uint32_t, uint8_t> from, std::pair<uint32_t, uint8_t> to)
    -> gc::Edge
{
    return gc::edge({from.first, gc::OutputPort{from.second}},
                    {to.first, gc::InputPort{to.second}});
}

auto test_graph_net_3x3()
    -> gc::Graph
{
    // 8 -> 7 -> 5
    //
    // |    |    |
    // v    v    v
    //
    // 6 -> 4 -> 2
    //
    // |    |    |
    // v    v    v
    //
    // 3 -> 1 -> 0
    return test_graph(
        {{2,0}, {2,1}, {2,1}, {1,1}, {2,2}, {1,1}, {1,2}, {1,2}, {0,2}},
        {edge({1,0}, {0,0}),
         edge({2,0}, {0,1}),
         edge({3,0}, {1,0}),
         edge({4,0}, {1,1}),
         edge({4,1}, {2,0}),
         edge({5,0}, {2,1}),
         edge({6,0}, {3,0}),
         edge({6,1}, {4,0}),
         edge({7,0}, {4,1}),
         edge({7,1}, {5,0}),
         edge({8,0}, {6,0}),
         edge({8,1}, {7,0})});
}

struct SourceInput final
{
    uint32_t node;
    gc::InputPort port;
    gc::Value value;
};

auto make_source_inputs(std::initializer_list<SourceInput> inputs)
    -> gc::SourceInputs
{
    auto result = gc::SourceInputs{};
    for (const auto& in : inputs)
    {
        add_to_last_group(result.destinations,
                          gc::EdgeInputEnd{in.node, in.port});
        next_group(result.destinations);
        result.values.push_back(in.value);
    }
    return result;
}

} // anonymous namespace

TEST(Gc, compile_inputless)
{
    check_comple_graph(
        test_graph(
            {},
            {}),
        "{}; []");

    // 0 -> 1 -> 2 -> 3
    check_comple_graph(
        test_graph(
            {{0,1}, {1,1}, {1,1}, {1,1}},
            {edge({0,0}, {1,0}),
             edge({1,0}, {2,0}),
             edge({2,0}, {3,0})}),
        "{(0) => ([O(0,0)->I(1,0)]) |"
        " (1) => ([O(1,0)->I(2,0)]) |"
        " (2) => ([O(2,0)->I(3,0)]) |"
        " (3)}; [(),(0),(1),(2)]");

    // 1 -> 2 -> 3 -> 0
    check_comple_graph(
        test_graph(
            {{1,0}, {0,1}, {1,1}, {1,1}},
            {edge({1,0}, {2,0}),
             edge({2,0}, {3,0}),
             edge({3,0}, {0,0})}),
        "{(1) => ([O(1,0)->I(2,0)]) |"
        " (2) => ([O(2,0)->I(3,0)]) |"
        " (3) => ([O(3,0)->I(0,0)]) |"
        " (0)}; [(3),(),(1),(2)]");

    // 0 -> 2 <- 1
    check_comple_graph(
        test_graph(
            {{0,1}, {0,1}, {2,0}},
            {edge({0,0}, {2,0}),
             edge({1,0}, {2,1})}),
        "{(0,1) => ([O(0,0)->I(2,0)],[O(1,0)->I(2,1)]) |"
        " (2)}; [(),(),(0,1)]");

    // 8 -> 7 -> 5
    //
    // |    |    |
    // v    v    v
    //
    // 6 -> 4 -> 2
    //
    // |    |    |
    // v    v    v
    //
    // 3 -> 1 -> 0
    check_comple_graph(
        test_graph_net_3x3(),
        "{(8) => ([O(8,0)->I(6,0)],[O(8,1)->I(7,0)]) |"
        " (6,7) => ([O(6,0)->I(3,0)],[O(6,1)->I(4,0)],[O(7,0)->I(4,1)],[O(7,1)->I(5,0)]) |"
        " (3,4,5) => ([O(3,0)->I(1,0)],[O(4,0)->I(1,1)],[O(4,1)->I(2,0)],[O(5,0)->I(2,1)]) |"
        " (1,2) => ([O(1,0)->I(0,0)],[O(2,0)->I(0,1)]) |"
        " (0)}; [(1,2),(3,4),(4,5),(6),(6,7),(7),(8),(8),()]");

    // Graph is not connected. Unreachable nodes are 1
    EXPECT_THROW(
        check_comple_graph(
            test_graph(
                {{0,1}, {2,1}},
                {edge({0,0}, {1,0}),
                 edge({1,0}, {1,1})}),
            ""),
        std::invalid_argument);

    // Edge end (1,1) refers to a non-existent input port
    EXPECT_THROW(
        check_comple_graph(
            test_graph(
                {{0,1}, {1,1}},
                {edge({0,0}, {1,0}),
                 edge({1,0}, {1,1})}),
            ""),
        std::invalid_argument);

    // Edge end (0,2) refers to a non-existent output port
    EXPECT_THROW(
        check_comple_graph(
            test_graph(
                {{0,1}, {1,1}},
                {edge({0,2}, {1,0}),
                 edge({1,0}, {1,1})}),
            ""),
        std::invalid_argument);

    // The following edges are not processed because
    // the graph has a cycle: [O(3,0)->I(2,0)]
    EXPECT_THROW(
        check_comple_graph(
            test_graph(
                {{1,0}, {0,1}, {1,1}, {1,1}},
                {edge({1,0}, {2,0}),
                 edge({2,0}, {3,0}),
                 edge({2,0}, {0,0}),
                 edge({3,0}, {2,0})}),
            ""),
        std::invalid_argument);

    // Edge end (2,0) is not the only one coming to the input port
    EXPECT_THROW(
        check_comple_graph(
            test_graph(
                {{0,1}, {0,1}, {1,0}},
                {edge({0,0}, {2,0}),
                 edge({1,0}, {2,0})}),
            ""),
        std::invalid_argument);
}

TEST(Gc, compile_with_inputs)
{
    // -> 0 -> 1 -> 2
    check_comple_graph(
        test_graph(
            {{1,1}, {1,1}, {1,1}},
            {edge({0,0}, {1,0}),
             edge({1,0}, {2,0})}),
        "{(0) => ([O(0,0)->I(1,0)]) |"
        " (1) => ([O(1,0)->I(2,0)]) |"
        " (2)}; [(),(0),(1)]",
        make_source_inputs({{
            .node = 0,
            .port = gc::InputPort{0},
            .value = 0 }}));
}

TEST(Gc, compile2)
{
    auto n0 = std::make_shared<TestNode>( 0, 1 );
    auto n1 = std::make_shared<TestNode>( 0, 1 );
    auto n2 = std::make_shared<TestNode>( 2, 0 );

    auto g = gc::Graph{
        .nodes = { n1, n2 },
        .edges = {edge({0, 0}, {2, 0}),
                  edge({1, 0}, {2, 1})}};

    // Throws because n0 is missing among graph nodes but is present
    // among graph edges.
    EXPECT_THROW(compile(g), std::out_of_range);
}

TEST(Gc, compute_1)
{
    auto g = test_graph(
        {{0, 1}, {1, 1}},
        {edge({0,0}, {1,0})});

    auto [instr, source_inputs] = compile(g);

    auto result = gc::ComputationResult{};
    compute(result, g, instr.get(), source_inputs);

    EXPECT_EQ(group(result.outputs, 0).size(), 1);
    EXPECT_EQ(group(result.outputs, 0)[0].as<int>(), 1);

    EXPECT_EQ(group(result.outputs, 1).size(), 1);
    EXPECT_EQ(group(result.outputs, 1)[0].as<int>(), 2);
}

TEST(Gc, compute_2)
{
    // 8 -> 7 -> 5
    //
    // |    |    |
    // v    v    v
    //
    // 6 -> 4 -> 2
    //
    // |    |    |
    // v    v    v
    //
    // 3 -> 1 -> 0
    auto g = test_graph_net_3x3();

    auto [instr, source_inputs] = gc::compile(g);

    auto format_result = [](const gc::ComputationResult& res)
        -> std::string
    {
        std::ostringstream s;
        for (size_t inode=0; inode<9; ++inode)
        {
            auto gr = group(res.outputs, inode);
            auto seq = std::ranges::transform_view(
                gr,
                [](const gc::Value& v)
                { return v.as<int>(); });
            s << inode << ": (" << common::format_seq(seq) << ')' << std::endl;
        }
        return s.str();
    };

    auto result = gc::ComputationResult{};
    compute(result, g, instr.get(), source_inputs);

    EXPECT_EQ(format_result(result), R"(0: ()
1: (7)
2: (9)
3: (3)
4: (3,5)
5: (3)
6: (2,2)
7: (3,2)
8: (1,2)
)");
}

TEST(Gc, compute_3)
{
    auto g = test_graph(
        {{2, 1}, {1, 1}},
        {edge({0,0}, {1,0})});

    auto [instr, source_inputs] = compile(g);

    auto result = gc::ComputationResult{};

    // Throws because one of source inputs refers to an inexistent node.
    source_inputs.destinations.values[0].node = 2;
    EXPECT_THROW(compute(result, g, instr.get(), source_inputs),
                 std::out_of_range);

    // Throws because one of source inputs refers to an inexistent port.
    source_inputs.destinations.values[0].node = 0;
    source_inputs.destinations.values[0].port.v = 2;
    EXPECT_THROW(compute(result, g, instr.get(), source_inputs),
                 std::out_of_range);

    // Not all external inputs are connected. We currently do not
    // detect it in `compute`. In this particular example, `compute` fails
    // because the empty input value is invalid and fails to cast to `int`.
    source_inputs.destinations.values[0].port.v = 1;
    EXPECT_THROW(compute(result, g, instr.get(), source_inputs),
                 std::bad_any_cast);

    // And if we feed all external inputs properly, `compute` will succeed.
    source_inputs.destinations.values[0].port.v = 0;
    source_inputs.values[0] = 123 - 1;
    source_inputs.values[1] = 321 - 1;
    compute(result, g, instr.get(), source_inputs);
    EXPECT_EQ(group(result.outputs, 1)[0].as<int>(), 444);
}

TEST(Gc, compute_partially)
{
    auto format_result = [](const gc::ComputationResult& res,
                            const gc::Graph& g)
        -> std::string
    {
        std::ostringstream s;
        auto node_count = g.nodes.size();
        for (size_t inode=0; inode<node_count; ++inode)
        {
            auto gr = group(res.outputs, inode);
            auto seq = std::ranges::transform_view(
                gr,
                [](const gc::Value& v)
                { return v.as<int>(); });
            const auto* node =
                static_cast<const TestNode*>(g.nodes.at(inode).get());
            s << inode << ": (" << common::format_seq(seq)
              << ") - ts: " << res.node_ts.at(inode)
              << ", computed: " << node->computation_count()
              << std::endl;
        }
        return s.str();
    };

    // [0]  [1]
    //  |    |
    //  0    1
    //  |    |
    //  +--. +--.
    //  |  | |  |
    //  v  v v  v
    //  2   3   4
    auto g = test_graph({{1, 1}, {1, 1},
                         {1, 1}, {2, 1}, {1, 1}},
                        {edge({0,0}, {2,0}),
                         edge({0,0}, {3,0}),
                         edge({1,0}, {3,1}),
                         edge({1,0}, {4,0})});

    auto [instr, source_inputs] = gc::compile(g);

    auto result = gc::ComputationResult{};

    compute(result, g, instr.get(), source_inputs);
    EXPECT_EQ(format_result(result, g), R"(0: (1) - ts: 1, computed: 1
1: (1) - ts: 1, computed: 1
2: (2) - ts: 1, computed: 1
3: (3) - ts: 1, computed: 1
4: (2) - ts: 1, computed: 1
)");

    // Update input [0]
    ++source_inputs.values[0].as<int>();

    compute(result, g, instr.get(), source_inputs);
    EXPECT_EQ(format_result(result, g), R"(0: (2) - ts: 2, computed: 2
1: (1) - ts: 1, computed: 1
2: (3) - ts: 2, computed: 2
3: (4) - ts: 2, computed: 2
4: (2) - ts: 1, computed: 1
)");

    // Update input [0] again
    ++source_inputs.values[0].as<int>();

    compute(result, g, instr.get(), source_inputs);
    EXPECT_EQ(format_result(result, g), R"(0: (3) - ts: 3, computed: 3
1: (1) - ts: 1, computed: 1
2: (4) - ts: 3, computed: 3
3: (5) - ts: 3, computed: 3
4: (2) - ts: 1, computed: 1
)");

    // Now update input [1]
    ++source_inputs.values[1].as<int>();

    compute(result, g, instr.get(), source_inputs);
    EXPECT_EQ(format_result(result, g), R"(0: (3) - ts: 3, computed: 3
1: (2) - ts: 4, computed: 2
2: (4) - ts: 3, computed: 3
3: (6) - ts: 4, computed: 4
4: (3) - ts: 4, computed: 2
)");
}
