#include "gc/graph_computation.hpp"

#include "common/format.hpp"

#include <gtest/gtest.h>

#include <initializer_list>
#include <numeric>
#include <ranges>


// using namespace std::string_view_literals;

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
    { std::fill(result.begin(), result.end(), 0); }

    auto compute_outputs(gc::ValueSpan result,
                         gc::ConstValueSpan inputs) const
        -> void override
    {
        std::iota(result.begin(), result.end(), 1);

        if (output_count_ == 0)
            return;

        auto output_index = 0;
        for (const auto& input : inputs)
        {
            result[output_index].as<int>() += input.as<int>();
            output_index = (output_index + 1) % output_count_;
        }
    }

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

    // 0 -> 2 <- 1
    check_comple_graph(
        test_graph(
            {{0,1}, {0,1}, {2,0}},
            {{{0,0}, {2,0}},
             {{1,0}, {2,1}}}),
        "{(0,1) => ([(0,0)->(2,0)],[(1,0)->(2,1)]) |"
        " (2)}");

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

struct MyStruct
{
    int foo;
    double bar;
    std::vector<unsigned int> flags;
};

constexpr inline auto fields_of(MyStruct& x)
    -> std::tuple<int&, double&, std::vector<unsigned int>&>
{ return { x.foo, x.bar, x.flags }; }

constexpr inline auto fields_of(const MyStruct& x)
    -> std::tuple<const int&, const double&, const std::vector<unsigned int>&>
{ return { x.foo, x.bar, x.flags }; }

inline auto fields_of(MyStruct&& x) = delete;

constexpr inline auto tuple_tag_of(common::Type_Tag<MyStruct>)
    -> common::Type_Tag<std::tuple<int, double, std::vector<unsigned int>>>
{ return {}; }

constexpr inline auto field_names_of(common::Type_Tag<MyStruct>)
    -> std::array<std::string_view, 3>
{ return { "foo", "bar", "flags" }; }



TEST(Gc, Type)
{
    const auto* t_int = gc::Type::of<int>();
    const auto* t_int_vec = gc::Type::of<std::vector<int>>();
    const auto* t_bool = gc::Type::of<bool>();
    const auto* t_int_vec_vec = gc::Type::of<std::vector<std::vector<int>>>();
    const auto* t_tuple = gc::Type::of<std::tuple<int, bool, std::vector<float>>>();
    const auto* t_struct = gc::Type::of<MyStruct>();
    const auto* t_path = gc::Type::of<gc::ValuePath>();

    EXPECT_EQ(gc::Type::of<std::vector<int>>(), t_int_vec);
    EXPECT_EQ(gc::Type::of<MyStruct>(), t_struct);

    using common::format;
    EXPECT_EQ(format(t_int),
              "Type{I32}");
    EXPECT_EQ(format(t_int_vec),
              "Type{Vector[I32]}");
    EXPECT_EQ(format(t_bool),
              "Type{Bool}");
    EXPECT_EQ(format(t_int_vec_vec),
              "Type{Vector[Vector[I32]]}");
    EXPECT_EQ(format(t_tuple),
              "Type{Tuple{I32, Bool, Vector[F32]}}");
    EXPECT_EQ(format(t_struct),
              "Type{Struct{foo: I32, bar: F64, flags: Vector[U32]}}");
    EXPECT_EQ(format(t_path), "Type{Path}");
}

class MyBlob final
{};

GC_REGISTER_CUSTOM_TYPE(MyBlob, 1);

TEST(Gc, CustomType)
{
    const auto* t_my_blob = gc::Type::of<MyBlob>();
    EXPECT_EQ(common::format(t_my_blob), "Type{Custom<MyBlob: 1>}");
}

// ---

TEST(Gc, Scalar)
{
    auto val = gc::Value(common::Type<int32_t>, 123);

    size_t visit_count{};
    gc::ScalarT{ val.type() }.visit(
        [&]<typename T>(common::Type_Tag<T>)
        {
            constexpr auto is_int32_t = std::is_same_v<T, int32_t>;
            EXPECT_TRUE(is_int32_t);
            ++visit_count;
            if constexpr (std::is_integral_v<T>)
                EXPECT_EQ(val.as<T>(), 123);
        });
    EXPECT_EQ(visit_count, 1);

    auto& ival = val.as<int32_t>();
    EXPECT_EQ(ival, 123);
}

TEST(Gc, DynamicValueAccess)
{
    auto v_int = gc::Value(123);
    EXPECT_EQ(v_int.get({}).as<int>(), 123);

    auto v_vec_double = gc::Value(std::vector<double>{ 1.2, 3.4, 5.6 });
    EXPECT_EQ(v_vec_double.get(gc::ValuePath{} / 1ul).as<double>(), 3.4);

    auto v_struct  = gc::Value(MyStruct{
        .foo = 123,
        .bar = 4.56,
        .flags = {12, 34, 56, 78, 90}
    });
    EXPECT_EQ(v_struct.get(gc::ValuePath{} / "foo").as<int>(), 123);
    EXPECT_EQ(v_struct.get(gc::ValuePath{} / "bar").as<double>(), 4.56);

    auto actual_flags =
        v_struct.get(gc::ValuePath{} / "flags").as<std::vector<unsigned int>>();
    auto expected_flags =
        std::vector<unsigned int>{12, 34, 56, 78, 90};
    EXPECT_EQ(actual_flags, expected_flags);

    EXPECT_EQ(v_struct.get(gc::ValuePath{} / "flags" / 0u).as<unsigned>(), 12);
    EXPECT_EQ(v_struct.get(gc::ValuePath{} / "flags" / 3u).as<unsigned>(), 78);
    EXPECT_EQ(v_struct.get(gc::ValuePath{} / "flags" / 4u).as<unsigned>(), 90);

    // vector::_M_range_check: __n (which is 5) >= this->size() (which is 5)
    EXPECT_THROW(v_struct.get(gc::ValuePath{} / "flags" / 5u),
                 std::out_of_range);

    v_struct.set(gc::ValuePath{} / "flags" / 3u, 912u);
    EXPECT_EQ(v_struct.get(gc::ValuePath{} / "flags" / 3u).as<unsigned>(), 912);

    v_struct.resize(gc::ValuePath{} / "flags", 6);
    v_struct.set(gc::ValuePath{} / "flags" / 5u, 144u);
    EXPECT_EQ(v_struct.get(gc::ValuePath{} / "flags" / 5u).as<unsigned>(), 144);
}

// ---

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
    auto g = test_graph(
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
