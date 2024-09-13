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
    check_comple_graph(
        test_graph(
            {},
            {}),
        "{}");

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
        " (3)}");

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
        " (0)}");

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
        " (0)}");

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

struct MyStruct
{
    int foo;
    double bar;
    std::vector<bool> flags;
};

constexpr inline auto fields_of(MyStruct& x)
    -> std::tuple<int&, double&, std::vector<bool>&>
{ return { x.foo, x.bar, x.flags }; }

constexpr inline auto fields_of(const MyStruct& x)
    -> std::tuple<const int&, const double&, const std::vector<bool>&>
{ return { x.foo, x.bar, x.flags }; }

inline auto fields_of(MyStruct&& x) = delete;

constexpr inline auto tuple_tag_of(common::Type_Tag<MyStruct>)
    -> common::Type_Tag<std::tuple<int, double, std::vector<bool>>>
{ return {}; }

constexpr inline auto field_names_of(common::Type_Tag<MyStruct>)
    -> std::array<std::string_view, 3>
{ return { "foo", "bar", "flags" }; }



TEST(Gc, Type)
{
    const auto* t_int = gc::Type::of<int>();
    const auto* t_int_vec = gc::Type::of<std::vector<int>>();
    const auto* t_bool = gc::Type::of<bool>();
    const auto* t_bool_bool_vec = gc::Type::of<std::vector<std::vector<bool>>>();
    const auto* t_tuple = gc::Type::of<std::tuple<int, bool, std::vector<float>>>();
    const auto* t_struct = gc::Type::of<MyStruct>();

    EXPECT_EQ(gc::Type::of<std::vector<int>>(), t_int_vec);
    EXPECT_EQ(gc::Type::of<MyStruct>(), t_struct);

    using common::format;
    EXPECT_EQ(format(t_int),
              "Type{I32}");
    EXPECT_EQ(format(t_int_vec),
              "Type{Vector[I32]}");
    EXPECT_EQ(format(t_bool),
              "Type{Bool}");
    EXPECT_EQ(format(t_bool_bool_vec),
              "Type{Vector[Vector[Bool]]}");
    EXPECT_EQ(format(t_tuple),
              "Type{Tuple{I32, Bool, Vector[F32]}}");
    EXPECT_EQ(format(t_struct),
              "Type{Struct{foo: I32, bar: F64, flags: Vector[Bool]}}");
}

class MyBlob final
{};

GC_REGISTER_CUSTOM_TYPE(MyBlob, 1);

TEST(Gc, CustomType)
{
    const auto* t_my_blob = gc::Type::of<MyBlob>();
    EXPECT_EQ(common::format(t_my_blob), "Type{Custom<MyBlob: 1>}");
}

TEST(Gc, compute)
{
    auto g = test_graph(
        {{0, 1}, {1, 1}},
        {{{0,0}, {1,0}}});

    auto val = gc::Value {
        .aggregate_type = gc::AggregateType::Scalar,
        .value = gc::Scalar{
            .type = gc::ScalarTypeId::I32,
            .value = { .i32 = 123 } } };

    auto& ival = as<int32_t>(val);
    EXPECT_EQ(ival, 123);

    auto instr = compile(g);

    auto result = gc::ComputationResult{};
    compute(result, g, instr.get());
}
