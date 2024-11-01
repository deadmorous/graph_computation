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
             uint32_t output_count,
             bool stateful = false) :
        state_inc_{stateful? 1: 0}
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
                         gc::ConstValueSpan inputs) const
        -> void override
    {
        ++computation_count_;

        std::iota(result.begin(), result.end(), 1 + state_);

        if (output_count() == 0)
            return;

        auto output_index = 0;
        for (const auto& input : inputs)
        {
            result[output_index].as<int>() += input.as<int>();
            output_index = (output_index + 1) % output_count();
        }

        state_ += state_inc_;
    }

    auto computation_count() const noexcept
        -> size_t
    { return computation_count_; }

private:
    int state_inc_;
    mutable int state_{};
    mutable size_t computation_count_{};
    gc::DynamicInputNames input_names_;
    gc::DynamicOutputNames output_names_;
};

struct TestGraphNodeSpec
{
    uint32_t input_count{};
    uint32_t output_count{};
    bool stateful{false};
};

struct TestGraphEdgeEndSpec
{
    uint32_t inode;
    uint32_t port;
};

struct TestGraphEdgeSpec
{
    TestGraphEdgeEndSpec from;
    TestGraphEdgeEndSpec to;
};

auto test_graph(std::initializer_list<TestGraphNodeSpec> nodes,
                std::initializer_list<TestGraphEdgeSpec> edges)
    -> gc::Graph
{
    auto result = gc::Graph{};
    for (const auto& node_spec : nodes)
        result.nodes.emplace_back(
            std::make_shared<TestNode>(node_spec.input_count,
                                       node_spec.output_count,
                                       node_spec.stateful));

    auto edge_end =
        [&](const TestGraphEdgeEndSpec& ee) -> gc::EdgeEnd
    { return { result.nodes.at(ee.inode).get(), ee.port }; };

    for (const auto& edge_spec : edges)
        result.edges.push_back(
            { edge_end(edge_spec.from), edge_end(edge_spec.to) });

    return result;
}

auto check_comple_graph(const gc::Graph& g,
                        std::string_view expected_formatted_instructions)
    -> void
{

    auto instructions = compile(g);

    // std::cout << "Compiled instructions: " << *instructions << std::endl;

    EXPECT_EQ(common::format(*instructions), expected_formatted_instructions);
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
        {{{1,0}, {0,0}},
         {{2,0}, {0,1}},
         {{3,0}, {1,0}},
         {{4,0}, {1,1}},
         {{4,1}, {2,0}},
         {{5,0}, {2,1}},
         {{6,0}, {3,0}},
         {{6,1}, {4,0}},
         {{7,0}, {4,1}},
         {{7,1}, {5,0}},
         {{8,0}, {6,0}},
         {{8,1}, {7,0}}});
}

} // anonymous namespace

TEST(Gc, compile)
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
            {{{0,0}, {1,0}},
             {{1,0}, {2,0}},
             {{2,0}, {3,0}}}),
        "{(0) => ([(0,0)->(1,0)]) |"
        " (1) => ([(1,0)->(2,0)]) |"
        " (2) => ([(2,0)->(3,0)]) |"
        " (3)}; [(),(0),(1),(2)]");

    // 1 -> 2 -> 3 -> 0
    check_comple_graph(
        test_graph(
            {{1,0}, {0,1}, {1,1}, {1,1}},
            {{{1,0}, {2,0}},
             {{2,0}, {3,0}},
             {{3,0}, {0,0}}}),
        "{(1) => ([(1,0)->(2,0)]) |"
        " (2) => ([(2,0)->(3,0)]) |"
        " (3) => ([(3,0)->(0,0)]) |"
        " (0)}; [(3),(),(1),(2)]");

    // 0 -> 2 <- 1
    check_comple_graph(
        test_graph(
            {{0,1}, {0,1}, {2,0}},
            {{{0,0}, {2,0}},
             {{1,0}, {2,1}}}),
        "{(0,1) => ([(0,0)->(2,0)],[(1,0)->(2,1)]) |"
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
        "{(8) => ([(8,0)->(6,0)],[(8,1)->(7,0)]) |"
        " (6,7) => ([(6,0)->(3,0)],[(6,1)->(4,0)],[(7,0)->(4,1)],[(7,1)->(5,0)]) |"
        " (3,4,5) => ([(3,0)->(1,0)],[(4,0)->(1,1)],[(4,1)->(2,0)],[(5,0)->(2,1)]) |"
        " (1,2) => ([(1,0)->(0,0)],[(2,0)->(0,1)]) |"
        " (0)}; [(1,2),(3,4),(4,5),(6),(6,7),(7),(8),(8),()]");

    // Graph is not connected. Unreachable nodes are 1
    EXPECT_THROW(
        check_comple_graph(
            test_graph(
                {{0,1}, {2,1}},
                {{{0,0}, {1,0}},
                 {{1,0}, {1,1}}}),
            ""),
        std::invalid_argument);

    // Edge end (1,1) refers to a non-existent input port
    EXPECT_THROW(
        check_comple_graph(
            test_graph(
                {{0,1}, {1,1}},
                {{{0,0}, {1,0}},
                 {{1,0}, {1,1}}}),
            ""),
        std::invalid_argument);

    // Edge end (0,2) refers to a non-existent output port
    EXPECT_THROW(
        check_comple_graph(
            test_graph(
                {{0,1}, {1,1}},
                {{{0,2}, {1,0}},
                 {{1,0}, {1,1}}}),
            ""),
        std::invalid_argument);

    // The following edges are not processed because
    // the graph has a cycle: [(3,0)->(2,0)]
    EXPECT_THROW(
        check_comple_graph(
            test_graph(
                {{1,0}, {0,1}, {1,1}, {1,1}},
                {{{1,0}, {2,0}},
                 {{2,0}, {3,0}},
                 {{2,0}, {0,0}},
                 {{3,0}, {2,0}}}),
            ""),
        std::invalid_argument);

    // Edge end (2,0) is not the only one coming to the input port
    EXPECT_THROW(
        check_comple_graph(
            test_graph(
                {{0,1}, {0,1}, {1,0}},
                {{{0,0}, {2,0}},
                 {{1,0}, {2,0}}}),
            ""),
        std::invalid_argument);
}

TEST(Gc, compile2)
{
    auto n0 = std::make_shared<TestNode>( 0, 1 );
    auto n1 = std::make_shared<TestNode>( 0, 1 );
    auto n2 = std::make_shared<TestNode>( 2, 0 );

    using EE = gc::EdgeEnd;

    auto g = gc::Graph{
        .nodes = { n1, n2 },
        .edges = {{EE{n0.get(), 0}, EE{n2.get(), 0}},
                  {EE{n1.get(), 0}, EE{n2.get(), 1}}}
    };

    // Throws because n0 is missing among graph nodes but is present
    // among graph edges.
    EXPECT_THROW(compile(g), std::out_of_range);
}

TEST(Gc, compute_1)
{
    auto g = test_graph(
        {{0, 1}, {1, 1}},
        {{{0,0}, {1,0}}});

    auto instr = compile(g);

    auto result = gc::ComputationResult{};
    compute(result, g, instr.get());

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

    auto instr = gc::compile(g);

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
    compute(result, g, instr.get());

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

    // stateful  stateless
    //       0*   1
    //       |    |
    //       +--. +--.
    //       |  | |  |
    //       v  v v  v
    //       2   3   4
    auto g = test_graph({{0, 1, true}, {0, 1, false},
                         {1, 1}, {2, 1}, {1, 1}},
                        {{{0,0}, {2,0}},
                         {{0,0}, {3,0}},
                         {{1,0}, {3,1}},
                         {{1,0}, {4,0}}});

    auto instr = gc::compile(g);

    auto result = gc::ComputationResult{};

    compute(result, g, instr.get());
    EXPECT_EQ(format_result(result, g), R"(0: (1) - ts: 1, computed: 1
1: (1) - ts: 1, computed: 1
2: (2) - ts: 1, computed: 1
3: (3) - ts: 1, computed: 1
4: (2) - ts: 1, computed: 1
)");

    compute(result, g, instr.get());
    EXPECT_EQ(format_result(result, g), R"(0: (2) - ts: 2, computed: 2
1: (1) - ts: 1, computed: 2
2: (3) - ts: 2, computed: 2
3: (4) - ts: 2, computed: 2
4: (2) - ts: 1, computed: 1
)");

    compute(result, g, instr.get());
    EXPECT_EQ(format_result(result, g), R"(0: (3) - ts: 3, computed: 3
1: (1) - ts: 1, computed: 3
2: (4) - ts: 3, computed: 3
3: (5) - ts: 3, computed: 3
4: (2) - ts: 1, computed: 1
)");
}
