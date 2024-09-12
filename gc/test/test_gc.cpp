#include "gc/graph_computation.hpp"

#include "common/format.hpp"

#include <gtest/gtest.h>

#include <initializer_list>

namespace {

class TestNode final
    : public gc::Node
{
public:
    TestNode(uint32_t input_count, uint32_t output_count) :
        input_count_{ input_count },
        output_count_{ output_count }
    {}

    auto input_count() const
        -> uint32_t override
    { return input_count_; }

    auto output_count() const
        -> uint32_t override
    { return output_count_; }

    auto default_inputs(gc::ValueSpan result) const
        -> void override
    {}

    auto compute_outputs(gc::ValueSpan result,
                         gc::ConstValueSpan inputs) const
        -> void override
    {}

private:
    uint32_t input_count_;
    uint32_t output_count_;
};

struct TestGraphNodeSpec
{
    uint32_t input_count;
    uint32_t output_count;
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
                                       node_spec.output_count));

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

} // anonymous namespace

TEST(Gc, compile)
{
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
        " (3)");

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
        " (0)");

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
        test_graph(
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
             {{8,1}, {7,0}}}),
        "{(8) => ([(8,0)->(6,0)],[(8,1)->(7,0)]) |"
        " (6,7) => ([(6,0)->(3,0)],[(6,1)->(4,0)],[(7,0)->(4,1)],[(7,1)->(5,0)]) |"
        " (3,4,5) => ([(3,0)->(1,0)],[(4,0)->(1,1)],[(4,1)->(2,0)],[(5,0)->(2,1)]) |"
        " (1,2) => ([(1,0)->(0,0)],[(2,0)->(0,1)]) |"
        " (0)");

    // TODO: Check exception
    // check_comple_graph(
    //     test_graph(
    //         {{1,0}, {0,1}, {1,1}, {1,1}},
    //         {{{1,0}, {2,0}},
    //          {{2,0}, {3,0}},
    //          {{2,0}, {0,0}},
    //          {{3,0}, {2,0}}}),
    //     "{(1) => ([(1,0)->(2,0)]) |"
    //     " (2) => ([(2,0)->(3,0)]) |"
    //     " (3) => ([(3,0)->(2,0)]) |"
    //     " (0)");

    // auto result = gc::ComputationResult{};
    // compute(result, g, instructions.get());
}
