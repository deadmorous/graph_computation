/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "common/detail/set_like.hpp"
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
static_assert(common::detail::SetLikeType<FruitFlags>);

} // anonymous namespace


TEST(Common_EnumFlags, Basic)
{
    auto flags = FruitFlags{Fruit::Apple, Fruit::Banana};

    EXPECT_EQ(common::format(flags), "{Apple, Banana}");
    EXPECT_TRUE(!flags.all());
    EXPECT_TRUE(flags.any());
    EXPECT_EQ(flags.size(), 2);
    EXPECT_FALSE(flags.empty());
    EXPECT_TRUE(flags & Fruit::Apple);
    EXPECT_TRUE(flags & Fruit::Banana);
    EXPECT_FALSE(flags & Fruit::Orange);

    flags &= ~FruitFlags{Fruit::Apple};
    EXPECT_EQ(common::format(flags), "{Banana}");
    EXPECT_TRUE(!flags.all());
    EXPECT_TRUE(flags.any());
    EXPECT_EQ(flags.size(), 1);
    EXPECT_FALSE(flags.empty());
    EXPECT_FALSE(flags & Fruit::Apple);
    EXPECT_TRUE(flags & Fruit::Banana);
    EXPECT_FALSE(flags & Fruit::Orange);

    flags.clear(Fruit::Banana);
    EXPECT_EQ(common::format(flags), "{}");
    EXPECT_TRUE(!flags.all());
    EXPECT_TRUE(!flags.any());
    EXPECT_EQ(flags.size(), 0);
    EXPECT_TRUE(flags.empty());
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
    EXPECT_EQ(flags.size(), 3);
    EXPECT_FALSE(flags.empty());
    EXPECT_TRUE(flags & Fruit::Apple);
    EXPECT_TRUE(flags & Fruit::Banana);
    EXPECT_TRUE(flags & Fruit::Orange);

    flags |= FruitFlags{Fruit::Apple, Fruit::Banana};
    EXPECT_EQ(flags, FruitFlags(Fruit::Apple, Fruit::Banana, Fruit::Orange));

    flags.clear();
    EXPECT_EQ(flags.size(), 0);
    EXPECT_TRUE(flags.empty());
}

TEST(Common_EnumFlags, Set)
{
    auto flags = FruitFlags{};

    EXPECT_TRUE(flags.empty());

    {
        auto [it, inserted] = flags.insert(Fruit::Banana);
        EXPECT_EQ(*it, Fruit::Banana);
        EXPECT_TRUE(inserted);
        ++it;
        EXPECT_EQ(it, flags.end());
        EXPECT_FALSE(flags.empty());
        EXPECT_EQ(std::distance(flags.begin(), flags.end()), 1);
    }

    {
        auto [it, inserted] = flags.insert(Fruit::Orange);
        EXPECT_EQ(*it, Fruit::Orange);
        EXPECT_TRUE(inserted);
        ++it;
        EXPECT_EQ(it, flags.end());
        EXPECT_FALSE(flags.empty());
        EXPECT_EQ(std::distance(flags.begin(), flags.end()), 2);
    }

    {
        auto [it, inserted] = flags.insert(Fruit::Banana);
        EXPECT_EQ(*it, Fruit::Banana);
        EXPECT_FALSE(inserted);
        ++it;
        EXPECT_EQ(*it, Fruit::Orange);
        ++it;
        EXPECT_EQ(it, flags.end());
        EXPECT_FALSE(flags.empty());
        EXPECT_EQ(std::distance(flags.begin(), flags.end()), 2);
    }

    {
        auto it = flags.find(Fruit::Banana);
        EXPECT_NE(it, flags.end());
        it = flags.erase(it);
        EXPECT_EQ(*it, Fruit::Orange);
        ++it;
        EXPECT_EQ(it, flags.end());
        EXPECT_FALSE(flags.empty());
        EXPECT_EQ(std::distance(flags.begin(), flags.end()), 1);
    }

    {
        auto [it, inserted] = flags.insert(Fruit::Apple);
        EXPECT_EQ(*it, Fruit::Apple);
        EXPECT_TRUE(inserted);
        ++it;
        EXPECT_EQ(*it, Fruit::Orange);
        ++it;
        EXPECT_EQ(it, flags.end());
        EXPECT_FALSE(flags.empty());
        EXPECT_EQ(std::distance(flags.begin(), flags.end()), 2);
    }

    {
        auto it = flags.find(Fruit::Apple);
        EXPECT_NE(it, flags.end());
        it = flags.erase(it);
        EXPECT_EQ(*it, Fruit::Orange);
        it = flags.erase(it);
        EXPECT_EQ(it, flags.end());
        EXPECT_TRUE(flags.empty());
        EXPECT_EQ(std::distance(flags.begin(), flags.end()), 0);
    }
}
