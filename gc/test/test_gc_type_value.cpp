/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024-2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "mpk/mix/struct_type_macro.hpp"
#include "mpk/mix/value/value.hpp"

#include <gtest/gtest.h>

#include <format>
#include <initializer_list>


using namespace std::literals;

namespace {

struct MyStruct
{
    int foo{};
    double bar{};
    std::vector<unsigned int> flags;
};

MPKMIX_STRUCT_TYPE(MyStruct, foo, bar, flags);

MPKMIX_STRONG_TYPE(MyIndex, uint32_t);

} // anonymous namespace


TEST(Gc, ActuallyEqualityComparable)
{
    using Vs = std::vector<MyStruct>;
    using Vi = std::vector<int>;
    static_assert(!mpk::mix::value::detail::actually_equality_comparable<MyStruct>);
    static_assert(!mpk::mix::value::detail::actually_equality_comparable<Vs>);
    static_assert(mpk::mix::value::detail::actually_equality_comparable<Vi>);
    static_assert(mpk::mix::value::detail::actually_equality_comparable<int>);
}

TEST(Gc, Type)
{
    const auto* t_int = mpk::mix::value::Type::of<int>();
    const auto* t_int_vec = mpk::mix::value::Type::of<std::vector<int>>();
    const auto* t_bool = mpk::mix::value::Type::of<bool>();
    const auto* t_int_vec_vec = mpk::mix::value::Type::of<std::vector<std::vector<int>>>();
    const auto* t_tuple = mpk::mix::value::Type::of<std::tuple<int, bool, std::vector<float>>>();
    const auto* t_struct = mpk::mix::value::Type::of<MyStruct>();
    const auto* t_path = mpk::mix::value::Type::of<mpk::mix::value::ValuePath>();
    const auto* t_struct_arr_3 = mpk::mix::value::Type::of<std::array<MyStruct, 3>>();

    EXPECT_EQ(mpk::mix::value::Type::of<std::vector<int>>(), t_int_vec);
    EXPECT_EQ(mpk::mix::value::Type::of<MyStruct>(), t_struct);

    EXPECT_EQ(std::format("{}", t_int),
              "Type{I32}");
    EXPECT_EQ(std::format("{}", t_int_vec),
              "Type{Vector[I32]}");
    EXPECT_EQ(std::format("{}", t_bool),
              "Type{Bool}");
    EXPECT_EQ(std::format("{}", t_int_vec_vec),
              "Type{Vector[Vector[I32]]}");
    EXPECT_EQ(std::format("{}", t_tuple),
              "Type{Tuple{I32, Bool, Vector[F32]}}");
    EXPECT_EQ(std::format("{}", t_struct),
              "Type{Struct{foo: I32, bar: F64, flags: Vector[U32]}}");
    EXPECT_EQ(std::format("{}", t_path),
              "Type{Path}");
    EXPECT_EQ(std::format("{}", t_struct_arr_3),
              "Type{Array<3>[Struct{foo: I32, bar: F64, flags: Vector[U32]}]}");
}

TEST(Gc, Array)
{
    using A = std::array<int, 3>;

    const auto* t_int_arr_3 = mpk::mix::value::Type::of<A>();
    EXPECT_EQ(std::format("{}", t_int_arr_3), "Type{Array<3>[I32]}");

    auto a = A{1, 4, 9};
    auto v = mpk::mix::value::Value{a};
    EXPECT_EQ(v.get(mpk::mix::value::ValuePath{0}), 1);
    EXPECT_EQ(v.get(mpk::mix::value::ValuePath{1}), 4);
    EXPECT_EQ(v.get(mpk::mix::value::ValuePath{2}), 9);

    EXPECT_EQ(std::format("{}", v), "[1,4,9]");
    EXPECT_EQ(v.size(), 3);
    EXPECT_THROW(v.resize(34), std::invalid_argument);
    EXPECT_EQ(v.size(), 3);

    v.set(mpk::mix::value::ValuePath{2}, 49);
    EXPECT_EQ(v.get(mpk::mix::value::ValuePath{2}), 49);
    EXPECT_EQ(std::format("{}", v), "[1,4,49]");
    EXPECT_THROW(v.set(mpk::mix::value::ValuePath{4}, 11), std::out_of_range);
}


enum class MyEnum : uint8_t
{
    Foo,
    Bar = 5,
    Baz
};

MPKMIX_VALUE_REGISTER_ENUM_TYPE(MyEnum, 1);

TEST(Gc, EnumType)
{
    using Underlying = std::underlying_type_t<MyEnum>;
    using UnderlyingVec = std::vector<Underlying>;
    using StringViewVec = std::vector<std::string_view>;

    // Type-specific checks

    const auto* t_my_enum = mpk::mix::value::Type::of<MyEnum>();
    EXPECT_EQ(std::format("{}", t_my_enum), "Type{Enum<MyEnum: 1>}");

    auto v0 = mpk::mix::value::Value::make(t_my_enum);

    auto names =
        v0.get(mpk::mix::value::ValuePath{ "names"sv }).as<StringViewVec>();
    auto expected_names = StringViewVec{ "Foo"sv, "Bar"sv, "Baz"sv };
    EXPECT_EQ(names, expected_names);

    auto values =
        v0.get(mpk::mix::value::ValuePath{ "values"sv }).as<UnderlyingVec>();
    auto expected_values = UnderlyingVec{ 0, 5, 6 };
    EXPECT_EQ(values, expected_values);

    // Value-specific checks

    auto v = mpk::mix::value::Value{MyEnum::Baz};

    auto e = v.as<MyEnum>();
    EXPECT_EQ(e, MyEnum::Baz);

    v.set({}, MyEnum::Bar);
    EXPECT_EQ(v, MyEnum::Bar);

    auto vi = v.get(mpk::mix::value::ValuePath{ "index"sv }).as<size_t>();
    EXPECT_EQ(vi, magic_enum::enum_index(MyEnum::Bar));

    auto vv = v.get(mpk::mix::value::ValuePath{ "value"sv }).as<Underlying>();
    EXPECT_EQ(vv, magic_enum::enum_integer(MyEnum::Bar));

    auto vn = v.get(mpk::mix::value::ValuePath{ "name"sv }).as<std::string_view>();
    EXPECT_EQ(vn, "Bar"sv);

    v.set(mpk::mix::value::ValuePath{ "index"sv }, size_t{1});
    EXPECT_EQ(std::format("{}", v), "Bar");

    v.set(mpk::mix::value::ValuePath{ "name"sv }, "Foo"s);
    EXPECT_EQ(v.as<MyEnum>(), MyEnum::Foo);

    v.set(mpk::mix::value::ValuePath{ "value"sv },
          mpk::mix::value::Value{ mpk::mix::Type<Underlying>, 6 });
    EXPECT_EQ(v.as<MyEnum>(), MyEnum::Baz);
}

TEST(Gc, EnumFlagsType)
{
    using MyFlags = mpk::mix::EnumFlags<MyEnum>;
    const auto* type = mpk::mix::value::Type::of<MyFlags>();
    EXPECT_EQ(std::format("{}", type), "Type{EnumFlags{Enum<MyEnum: 1>}}");

    auto flags = MyFlags{MyEnum::Foo};
    auto v = mpk::mix::value::Value{flags};

    EXPECT_EQ(v.as<MyFlags>(), flags);
    EXPECT_EQ(v.size(), 1);
    EXPECT_TRUE(v.contains(MyEnum::Foo));
    EXPECT_FALSE(v.contains(MyEnum::Bar));
    EXPECT_FALSE(v.contains(MyEnum::Baz));
    EXPECT_EQ(std::format("{}", v), std::format("{}", flags));

    flags |= MyEnum::Baz;
    v.insert(MyEnum::Baz);
    EXPECT_EQ(v.as<MyFlags>(), flags);
    EXPECT_EQ(v.size(), 2);
    EXPECT_TRUE(v.contains(MyEnum::Foo));
    EXPECT_FALSE(v.contains(MyEnum::Bar));
    EXPECT_TRUE(v.contains(MyEnum::Baz));
    EXPECT_EQ(std::format("{}", v), std::format("{}", flags));
    EXPECT_EQ(std::format("{}", flags), "{Foo, Baz}");

    {
        auto keys = v.keys();
        ASSERT_EQ(keys.size(), 2);
        EXPECT_EQ(keys[0].as<MyEnum>(), MyEnum::Foo);
        EXPECT_EQ(keys[1].as<MyEnum>(), MyEnum::Baz);
    }

    flags &= ~MyFlags{MyEnum::Foo};
    v.remove(MyEnum::Foo);
    EXPECT_EQ(v.as<MyFlags>(), flags);
    EXPECT_EQ(std::format("{}", v), std::format("{}", flags));
    EXPECT_EQ(std::format("{}", flags), "{Baz}");

    {
        auto keys = v.keys();
        ASSERT_EQ(keys.size(), 1);
        EXPECT_EQ(keys[0].as<MyEnum>(), MyEnum::Baz);
    }

    v.set_default();
    EXPECT_EQ(v.size(), 0);
    EXPECT_FALSE(v.contains(MyEnum::Foo));
}

class MyBlob final
{};

MPKMIX_VALUE_REGISTER_CUSTOM_TYPE(MyBlob, 1);

TEST(Gc, CustomType)
{
    const auto* t_my_blob = mpk::mix::value::Type::of<MyBlob>();
    EXPECT_EQ(std::format("{}", t_my_blob), "Type{Custom<MyBlob: 1>}");
}

// ---

TEST(Gc, Scalar)
{
    auto val = mpk::mix::value::Value(mpk::mix::Type<int32_t>, 123);

    size_t visit_count{};
    mpk::mix::value::ScalarT{ val.type() }.visit(
        [&]<typename T>(mpk::mix::Type_Tag<T>)
        {
            constexpr auto is_int32_t = std::is_same_v<T, int32_t>;
            EXPECT_TRUE(is_int32_t);
            ++visit_count;
            if constexpr (std::is_integral_v<T>)
            {
                EXPECT_EQ(val.as<T>(), 123);
            }
        });
    EXPECT_EQ(visit_count, 1);

    auto& ival = val.as<int32_t>();
    EXPECT_EQ(ival, 123);
}

TEST(Gc, String)
{
    auto check = [](const mpk::mix::value::Value& val,
                    auto tag,
                    std::string_view expected_val)
    {
        EXPECT_EQ(val.type(), mpk::mix::value::type_of(tag));
        EXPECT_EQ(val.as(tag), expected_val);

        auto as_s = val.convert_to<std::string>();
        static_assert(std::same_as<decltype(as_s), std::string>);
        EXPECT_EQ(as_s, expected_val);

        auto as_sv = val.convert_to<std::string_view>();
        static_assert(std::same_as<decltype(as_sv), std::string_view>);
        EXPECT_EQ(as_sv, expected_val);

        EXPECT_THROW(val.convert_to<int>(), std::invalid_argument);
    };

    constexpr auto ts = mpk::mix::Type<std::string>;
    constexpr auto tsv = mpk::mix::Type<std::string_view>;
    constexpr auto sv1 = "asd"sv;
    constexpr auto sv2 = "qwe"sv;

    auto v1_s_from_s = mpk::mix::value::Value(ts, std::string(sv1));
    auto v2_sv_from_sv = mpk::mix::value::Value(tsv, sv2);

    // Should NOT compile
    // auto v2_sv_from_s = mpk::mix::value::Value(tsv, std::string(sv2));

    // Should compile
    auto v2_s_from_sv = mpk::mix::value::Value(ts, sv2);

    check(v1_s_from_s, ts, sv1);
    check(v2_sv_from_sv, tsv, sv2);
    check(v2_s_from_sv, ts, sv2);
}

TEST(Gc, DynamicValueAccess)
{
    auto v_int = mpk::mix::value::Value(123);
    EXPECT_EQ(v_int.get({}).as<int>(), 123);

    auto v_vec_double = mpk::mix::value::Value(std::vector<double>{ 1.2, 3.4, 5.6 });
    EXPECT_EQ(v_vec_double.get(mpk::mix::value::ValuePath{} / 1ul).as<double>(), 3.4);

    auto v_struct  = mpk::mix::value::Value(MyStruct{
        .foo = 123,
        .bar = 4.56,
        .flags = {12, 34, 56, 78, 90}
    });
    EXPECT_EQ(v_struct.get(mpk::mix::value::ValuePath{} / "foo"sv).as<int>(), 123);
    EXPECT_EQ(v_struct.get(mpk::mix::value::ValuePath{} / "bar"sv).as<double>(), 4.56);

    auto actual_flags =
        v_struct.get(mpk::mix::value::ValuePath{} / "flags"sv)
                .as<std::vector<unsigned int>>();
    auto expected_flags =
        std::vector<unsigned int>{12, 34, 56, 78, 90};
    EXPECT_EQ(actual_flags, expected_flags);

    EXPECT_EQ(v_struct.get(mpk::mix::value::ValuePath{} / "flags"sv / 0u).as<unsigned>(),
              12);
    EXPECT_EQ(v_struct.get(mpk::mix::value::ValuePath{} / "flags"sv / 3u).as<unsigned>(),
              78);
    EXPECT_EQ(v_struct.get(mpk::mix::value::ValuePath{} / "flags"sv / 4u).as<unsigned>(),
              90);

    // vector::_M_range_check: __n (which is 5) >= this->size() (which is 5)
    EXPECT_THROW(v_struct.get(mpk::mix::value::ValuePath{} / "flags"sv / 5u),
                 std::out_of_range);

    v_struct.set(mpk::mix::value::ValuePath{} / "flags"sv / 3u, 912u);
    EXPECT_EQ(v_struct.get(mpk::mix::value::ValuePath{} / "flags"sv / 3u).as<unsigned>(),
              912);

    v_struct.resize(mpk::mix::value::ValuePath{} / "flags"sv, 6);
    v_struct.set(mpk::mix::value::ValuePath{} / "flags"sv / 5u, 144u);
    EXPECT_EQ(v_struct.get(mpk::mix::value::ValuePath{} / "flags"sv / 5u).as<unsigned>(),
              144);
}

TEST(Gc, ValueReflection)
{
    auto v_struct  = mpk::mix::value::Value(MyStruct{
        .foo = 123,
        .bar = 4.56,
        .flags = {12, 34, 56, 78, 90}
    });

    // Key extraction
    auto struct_keys = v_struct.path_item_keys();
    EXPECT_EQ(struct_keys.size(), 3);
    EXPECT_EQ(struct_keys[0], mpk::mix::value::ValuePathItem("foo"sv));
    EXPECT_EQ(struct_keys[1], mpk::mix::value::ValuePathItem("bar"sv));
    EXPECT_EQ(struct_keys[2], mpk::mix::value::ValuePathItem("flags"sv));

    auto v_flags = v_struct.get(mpk::mix::value::ValuePath{} / "flags"sv);
    auto flags_keys = v_flags.path_item_keys();
    EXPECT_EQ(flags_keys.size(), 5);
    EXPECT_EQ(flags_keys[0], mpk::mix::value::ValuePathItem(0u));
    EXPECT_EQ(flags_keys[1], mpk::mix::value::ValuePathItem(1u));
    EXPECT_EQ(flags_keys[2], mpk::mix::value::ValuePathItem(2u));
    EXPECT_EQ(flags_keys[3], mpk::mix::value::ValuePathItem(3u));
    EXPECT_EQ(flags_keys[4], mpk::mix::value::ValuePathItem(4u));

    auto v_tuple = mpk::mix::value::Value(std::make_tuple(1, 2.3));
    auto tuple_keys = v_tuple.path_item_keys();
    EXPECT_EQ(tuple_keys.size(), 2);
    EXPECT_EQ(tuple_keys[0], mpk::mix::value::ValuePathItem(0u));
    EXPECT_EQ(tuple_keys[1], mpk::mix::value::ValuePathItem(1u));

    // Dynamic construction of default value from type
    auto v_struct2 = mpk::mix::value::Value::make(v_struct.type());
    EXPECT_EQ(v_struct2.type(), v_struct.type());
    auto typed_struct2 = v_struct2.as<MyStruct>();
    EXPECT_EQ(typed_struct2.foo, 0);
    EXPECT_EQ(typed_struct2.bar, 0);
    EXPECT_EQ(typed_struct2.flags.size(), 0);

    auto struct2_keys = v_struct2.path_item_keys();
    EXPECT_EQ(struct2_keys.size(), 3);
    EXPECT_EQ(struct2_keys[0], mpk::mix::value::ValuePathItem("foo"sv));
    EXPECT_EQ(struct2_keys[1], mpk::mix::value::ValuePathItem("bar"sv));
    EXPECT_EQ(struct2_keys[2], mpk::mix::value::ValuePathItem("flags"sv));
}

TEST(Gc, StrongType)
{
    static_assert(std::same_as<MyIndex::Weak, uint32_t>);

    const auto* type = mpk::mix::value::type_of<MyIndex>();
    EXPECT_EQ(std::format("{}", type), "Type{Strong{U32}}"sv);

    auto my_index = MyIndex{123};
    auto v = mpk::mix::value::Value{ my_index };
    EXPECT_EQ(v.as<MyIndex>(), my_index);

    const auto path = mpk::mix::value::ValuePath{} / "v"sv;
    EXPECT_EQ(v.get(path).as<MyIndex::Weak>(), my_index.v);

    v.set(path, MyIndex::Weak{456});
    EXPECT_EQ(v.get(path).as<MyIndex::Weak>(), 456);
    EXPECT_EQ(v.as<MyIndex>(), MyIndex{456});

    auto v1 = mpk::mix::value::Value::make(type);
    EXPECT_EQ(v1.as<MyIndex>(), MyIndex{0});
}

TEST(Gc, FormatValue)
{
    // EXPECT_EQ(std::format("{}", mpk::mix::value::Value(123)), "123");
    EXPECT_EQ(std::format("{}", mpk::mix::value::Value(mpk::mix::Type<uint8_t>, 123)), "123");

    // int8_t is formatted as int, but it's not the same as char - we don't
    // currently support char
    EXPECT_EQ(std::format("{}", mpk::mix::value::Value(mpk::mix::Type<int8_t>, 'A')), "65");

    // TODO: Uncomment when we support char
    // EXPECT_EQ(std::format("{}", mpk::mix::value::Value(mpk::mix::Type<char>, 'A')), "A");

    EXPECT_EQ(std::format("{}", mpk::mix::value::Value(-1.23)), "-1.23");
    EXPECT_EQ(std::format("{}", mpk::mix::value::Value(std::byte{0x9c})), "9c");

    EXPECT_EQ(std::format("{}", mpk::mix::value::Value(true)), "true");
    EXPECT_EQ(std::format("{}", mpk::mix::value::Value(false)), "false");

    EXPECT_EQ(std::format("{}", mpk::mix::value::Value("Hello"sv)), "Hello");
    EXPECT_EQ(std::format("{}", mpk::mix::value::Value("World"s)), "World");

    EXPECT_EQ(std::format("{}", mpk::mix::value::Value(MyIndex{534})), "534");

    auto v = std::vector<int>{9,8,75};
    EXPECT_EQ(std::format("{}", mpk::mix::value::Value(v)), "[9,8,75]");

    auto t = std::make_tuple(1, 2.3, true, "hello"sv);
    EXPECT_EQ(std::format("{}", mpk::mix::value::Value(t)), "{1,2.3,true,hello}");

    auto s = MyStruct
    {
        .foo = 345,
        .bar = 1.3e11,
        .flags = {1, 3, 7, 13, 23}
    };
    EXPECT_EQ(std::format("{}", mpk::mix::value::Value(s)),
              "{foo=345,bar=1.3e+11,flags=[1,3,7,13,23]}");

    EXPECT_EQ(std::format("{}", mpk::mix::value::Value(MyBlob{})), "custom");
}

TEST(Gc, ValueEquality)
{
    EXPECT_EQ(mpk::mix::value::Value{123}, mpk::mix::value::Value{123});
    EXPECT_NE(mpk::mix::value::Value{123}, mpk::mix::value::Value{123u});
    EXPECT_NE(mpk::mix::value::Value{123}, mpk::mix::value::Value{456});
    auto myval_1a = MyStruct{
        .foo = 1,
        .bar = 2.34,
        .flags = {4, 8}
    };
    auto myval_1b = myval_1a;
    auto myval_2 = MyStruct{
        .foo = 1,
        .bar = 2.34,
        .flags = {4, 8, 16}
    };
    EXPECT_EQ(mpk::mix::value::Value{myval_1a}, mpk::mix::value::Value{myval_1b});
    EXPECT_NE(mpk::mix::value::Value{myval_1a}, mpk::mix::value::Value{myval_2});
}
