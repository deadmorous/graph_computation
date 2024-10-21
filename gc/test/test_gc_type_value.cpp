#include "gc/struct_type_macro.hpp"
#include "gc/value.hpp"

#include "common/format.hpp"

#include <gtest/gtest.h>

#include <initializer_list>


using namespace std::literals;

namespace {

struct MyStruct
{
    int foo{};
    double bar{};
    std::vector<unsigned int> flags;
};

GCLIB_STRUCT_TYPE(MyStruct, foo, bar, flags);

GCLIB_STRONG_TYPE(MyIndex, uint32_t);

} // anonymous namespace


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

TEST(Gc, String)
{
    auto check = [](const gc::Value& val,
                    auto tag,
                    std::string_view expected_val)
    {
        EXPECT_EQ(val.type(), gc::type_of(tag));
        EXPECT_EQ(val.as(tag), expected_val);

        auto as_s = val.convert_to<std::string>();
        static_assert(std::same_as<decltype(as_s), std::string>);
        EXPECT_EQ(as_s, expected_val);

        auto as_sv = val.convert_to<std::string_view>();
        static_assert(std::same_as<decltype(as_sv), std::string_view>);
        EXPECT_EQ(as_sv, expected_val);

        EXPECT_THROW(val.convert_to<int>(), std::invalid_argument);
    };

    constexpr auto ts = common::Type<std::string>;
    constexpr auto tsv = common::Type<std::string_view>;
    constexpr auto sv1 = "asd"sv;
    constexpr auto sv2 = "qwe"sv;

    auto v1_s_from_s = gc::Value(ts, std::string(sv1));
    auto v2_sv_from_sv = gc::Value(tsv, sv2);

    // Should NOT compile
    // auto v2_sv_from_s = gc::Value(tsv, std::string(sv2));

    // Should compile
    auto v2_s_from_sv = gc::Value(ts, sv2);

    check(v1_s_from_s, ts, sv1);
    check(v2_sv_from_sv, tsv, sv2);
    check(v2_s_from_sv, ts, sv2);
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
    EXPECT_EQ(v_struct.get(gc::ValuePath{} / "foo"sv).as<int>(), 123);
    EXPECT_EQ(v_struct.get(gc::ValuePath{} / "bar"sv).as<double>(), 4.56);

    auto actual_flags =
        v_struct.get(gc::ValuePath{} / "flags"sv)
                .as<std::vector<unsigned int>>();
    auto expected_flags =
        std::vector<unsigned int>{12, 34, 56, 78, 90};
    EXPECT_EQ(actual_flags, expected_flags);

    EXPECT_EQ(v_struct.get(gc::ValuePath{} / "flags"sv / 0u).as<unsigned>(),
              12);
    EXPECT_EQ(v_struct.get(gc::ValuePath{} / "flags"sv / 3u).as<unsigned>(),
              78);
    EXPECT_EQ(v_struct.get(gc::ValuePath{} / "flags"sv / 4u).as<unsigned>(),
              90);

    // vector::_M_range_check: __n (which is 5) >= this->size() (which is 5)
    EXPECT_THROW(v_struct.get(gc::ValuePath{} / "flags"sv / 5u),
                 std::out_of_range);

    v_struct.set(gc::ValuePath{} / "flags"sv / 3u, 912u);
    EXPECT_EQ(v_struct.get(gc::ValuePath{} / "flags"sv / 3u).as<unsigned>(),
              912);

    v_struct.resize(gc::ValuePath{} / "flags"sv, 6);
    v_struct.set(gc::ValuePath{} / "flags"sv / 5u, 144u);
    EXPECT_EQ(v_struct.get(gc::ValuePath{} / "flags"sv / 5u).as<unsigned>(),
              144);
}

TEST(Gc, ValueReflection)
{
    auto v_struct  = gc::Value(MyStruct{
        .foo = 123,
        .bar = 4.56,
        .flags = {12, 34, 56, 78, 90}
    });

    // Key extraction
    auto struct_keys = v_struct.keys();
    EXPECT_EQ(struct_keys.size(), 3);
    EXPECT_EQ(struct_keys[0], gc::ValuePathItem("foo"sv));
    EXPECT_EQ(struct_keys[1], gc::ValuePathItem("bar"sv));
    EXPECT_EQ(struct_keys[2], gc::ValuePathItem("flags"sv));

    auto v_flags = v_struct.get(gc::ValuePath{} / "flags"sv);
    auto flags_keys = v_flags.keys();
    EXPECT_EQ(flags_keys.size(), 5);
    EXPECT_EQ(flags_keys[0], gc::ValuePathItem(0u));
    EXPECT_EQ(flags_keys[1], gc::ValuePathItem(1u));
    EXPECT_EQ(flags_keys[2], gc::ValuePathItem(2u));
    EXPECT_EQ(flags_keys[3], gc::ValuePathItem(3u));
    EXPECT_EQ(flags_keys[4], gc::ValuePathItem(4u));

    auto v_tuple = gc::Value(std::make_tuple(1, 2.3));
    auto tuple_keys = v_tuple.keys();
    EXPECT_EQ(tuple_keys.size(), 2);
    EXPECT_EQ(tuple_keys[0], gc::ValuePathItem(0u));
    EXPECT_EQ(tuple_keys[1], gc::ValuePathItem(1u));

    // Dynamic construction of default value from type
    auto v_struct2 = gc::Value::make(v_struct.type());
    EXPECT_EQ(v_struct2.type(), v_struct.type());
    auto typed_struct2 = v_struct2.as<MyStruct>();
    EXPECT_EQ(typed_struct2.foo, 0);
    EXPECT_EQ(typed_struct2.bar, 0);
    EXPECT_EQ(typed_struct2.flags.size(), 0);

    auto struct2_keys = v_struct2.keys();
    EXPECT_EQ(struct2_keys.size(), 3);
    EXPECT_EQ(struct2_keys[0], gc::ValuePathItem("foo"sv));
    EXPECT_EQ(struct2_keys[1], gc::ValuePathItem("bar"sv));
    EXPECT_EQ(struct2_keys[2], gc::ValuePathItem("flags"sv));
}

TEST(Gc, StrongType)
{
    static_assert(std::same_as<MyIndex::Weak, uint32_t>);

    const auto* type = gc::type_of<MyIndex>();
    EXPECT_EQ(common::format(type), "Type{Strong{U32}}"sv);

    auto my_index = MyIndex{123};
    auto v = gc::Value{ my_index };
    EXPECT_EQ(v.as<MyIndex>(), my_index);

    const auto path = gc::ValuePath{} / "v"sv;
    EXPECT_EQ(v.get(path).as<MyIndex::Weak>(), my_index.v);

    v.set(path, MyIndex::Weak{456});
    EXPECT_EQ(v.get(path).as<MyIndex::Weak>(), 456);
    EXPECT_EQ(v.as<MyIndex>(), MyIndex{456});

    auto v1 = gc::Value::make(type);
    EXPECT_EQ(v1.as<MyIndex>(), MyIndex{0});
}
