/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "common/enum_flags.hpp"

#include <gtest/gtest.h>

#include <cstdint>


namespace {

enum class Fruit : uint8_t
{
    Apple,
    Banana,
    Orange
};

using FruitFlags = common::EnumFlags<Fruit>;

static_assert(common::EnumFlagsType<FruitFlags>);

} // anonymous namespace


TEST(Common_EnumFlags, Basic)
{
    auto flags = FruitFlags{Fruit::Apple, Fruit::Banana};

    EXPECT_EQ(common::format(flags), "{Apple, Banana}");
    EXPECT_TRUE(!flags.all());
    EXPECT_TRUE(flags.any());
    EXPECT_TRUE(flags & Fruit::Apple);
    EXPECT_TRUE(flags & Fruit::Banana);
    EXPECT_FALSE(flags & Fruit::Orange);

    flags &= ~FruitFlags{Fruit::Apple};
    EXPECT_EQ(common::format(flags), "{Banana}");
    EXPECT_TRUE(!flags.all());
    EXPECT_TRUE(flags.any());
    EXPECT_FALSE(flags & Fruit::Apple);
    EXPECT_TRUE(flags & Fruit::Banana);
    EXPECT_FALSE(flags & Fruit::Orange);

    flags.clear(Fruit::Banana);
    EXPECT_EQ(common::format(flags), "{}");
    EXPECT_TRUE(!flags.all());
    EXPECT_TRUE(!flags.any());
    EXPECT_FALSE(flags & Fruit::Apple);
    EXPECT_FALSE(flags & Fruit::Banana);
    EXPECT_FALSE(flags & Fruit::Orange);

    flags ^= Fruit::Orange;
    EXPECT_TRUE(flags & Fruit::Orange);
    EXPECT_EQ(common::format(flags), "{Orange}");

    flags ^= Fruit::Orange;
    EXPECT_FALSE(flags & Fruit::Orange);
    EXPECT_EQ(common::format(flags), "{}");

    flags ^= Fruit::Orange;
    EXPECT_TRUE(flags & Fruit::Orange);
    EXPECT_EQ(common::format(flags), "{Orange}");

    flags |= FruitFlags{Fruit::Apple, Fruit::Banana};
    EXPECT_EQ(common::format(flags), "{Apple, Banana, Orange}");
    EXPECT_TRUE(flags.all());
    EXPECT_TRUE(flags.any());
    EXPECT_TRUE(flags & Fruit::Apple);
    EXPECT_TRUE(flags & Fruit::Banana);
    EXPECT_TRUE(flags & Fruit::Orange);

    flags |= FruitFlags{Fruit::Apple, Fruit::Banana};
    EXPECT_EQ(flags, FruitFlags(Fruit::Apple, Fruit::Banana, Fruit::Orange));
}
