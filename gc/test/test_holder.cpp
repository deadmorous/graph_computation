/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "common/holder.hpp"

#include <gtest/gtest.h>


namespace {

template <common::HolderType H>
auto held_value(H& h) -> typename H::value_type&
{
    return h.value();
}

template <common::HolderType H>
auto const_held_value(const H& h) -> const typename H::value_type&
{
    return h.value();
}

} // anonymous namespace


TEST(HolderTest, ValueHolder)
{
    using H = common::ValueHolder<int>;
    static_assert(common::HolderType<H>);
    static_assert(std::same_as<H::value_type, int>);

    // Test construction
    auto h1 = H{};
    auto h2 = H{123};

    EXPECT_EQ(held_value(h1), int{});
    EXPECT_EQ(const_held_value(h1), int{});

    EXPECT_EQ(held_value(h2), 123);
    EXPECT_EQ(const_held_value(h2), 123);

    // Test modification
    h2.value() = 456;
    EXPECT_EQ(held_value(h2), 456);
    EXPECT_EQ(const_held_value(h2), 456);

    // Test copying
    auto h3 = h2;
    EXPECT_EQ(h3.value(), 456);
    h2.value() = 4561;
    EXPECT_EQ(h2.value(), 4561);
    EXPECT_EQ(h3.value(), 456);
    h3.value() = 4562;
    EXPECT_EQ(h2.value(), 4561);
    EXPECT_EQ(h3.value(), 4562);
}

TEST(HolderTest, ReferenceHolder)
{
    using H = common::ReferenceHolder<int>;
    static_assert(common::HolderType<H>);
    static_assert(std::same_as<H::value_type, int>);

    auto i1 = int{};
    auto i2 = 123;

    // Test construction
    auto h1 = H{i1};
    auto h2 = H{i2};

    EXPECT_EQ(held_value(h1), int{});
    EXPECT_EQ(const_held_value(h1), int{});

    EXPECT_EQ(held_value(h2), 123);
    EXPECT_EQ(const_held_value(h2), 123);

    // Test modification
    h2.value() = 456;
    EXPECT_EQ(held_value(h2), 456);
    EXPECT_EQ(const_held_value(h2), 456);
    EXPECT_EQ(i2, 456);

    i2 = 789;
    EXPECT_EQ(held_value(h2), 789);
    EXPECT_EQ(const_held_value(h2), 789);

    // No copying
    // [[maybe_unused]] auto h3 = h2;  // This fails to compile
    // [[maybe_unused]] h2 = h1;       // This fails to compile
}

TEST(HolderTest, BaseOf)
{
    struct X : common::BaseOf<X>
    {
        int v{};
    };
    using H = common::BaseOf<X>;
    static_assert(common::HolderType<H>);
    static_assert(std::same_as<H::value_type, X>);

    // Test construction
    auto h1 = X{};
    auto h2 = X{ .v = 123 };

    EXPECT_EQ(held_value(h1).v, int{});
    EXPECT_EQ(const_held_value(h1).v, int{});

    EXPECT_EQ(held_value(h2).v, 123);
    EXPECT_EQ(const_held_value(h2).v, 123);

    // Test modification
    h2.value() = X{ .v = 456 };
    EXPECT_EQ(held_value(h2).v, 456);
    EXPECT_EQ(const_held_value(h2).v, 456);

    // Test copying
    auto h3 = h2;
    EXPECT_EQ(h3.value().v, 456);
    h2.value().v = 4561;
    EXPECT_EQ(h2.value().v, 4561);
    EXPECT_EQ(h3.value().v, 456);
    h3.value().v = 4562;
    EXPECT_EQ(h2.value().v, 4561);
    EXPECT_EQ(h3.value().v, 4562);
}
