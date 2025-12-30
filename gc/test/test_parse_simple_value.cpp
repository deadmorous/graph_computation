/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "gc/parse_simple_value.hpp"
#include "gc/type.hpp"
#include "gc/value.hpp"

#include <gtest/gtest.h>


using namespace std::string_view_literals;

namespace {

enum class MyEnum : uint8_t
{
    Foo,
    Bar = 5,
    Baz
};

}

GCLIB_REGISTER_ENUM_TYPE(MyEnum, 1);

TEST(Gc_ParseSimpleValue, Enum)
{
    const auto* type = gc::Type::of<MyEnum>();

    EXPECT_EQ(gc::parse_simple_value("Foo"sv, type).as<MyEnum>(), MyEnum::Foo);
    EXPECT_EQ(gc::parse_simple_value("Bar"sv, type).as<MyEnum>(), MyEnum::Bar);
    EXPECT_EQ(gc::parse_simple_value("Baz"sv, type).as<MyEnum>(), MyEnum::Baz);
    EXPECT_THROW(gc::parse_simple_value("asd"sv, type), std::runtime_error);
}

// TODO: Add more tests here
