/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "common/linked_list.hpp"

#include <gtest/gtest.h>

#include <vector>


namespace {

using IntListItem = common::ValueLinkedListItem<int>;
using IntList = common::ValueLinkedList<int>;

using intvec = std::vector<int>;

auto to_int(int x) -> int { return x; }

struct X : common::IntrusiveLinkedListConnectivity<X>
{
    int v{};
};

auto to_int(const X& x) -> int { return x.v; }

using XList = common::IntrusiveLinkedList<X>;
using xvec = std::vector<X>;


auto v(auto... args) -> intvec
{ return { args... }; }


template <typename List>
auto values(const List& list) -> intvec
{
    auto result = intvec{};
    for (const auto& item : list)
        result.push_back(to_int(item.value()));
    return result;
};

template <common::LinkedListConnectivityType T>
auto loop(const T& item) -> intvec
{
    intvec result;
    const auto* start = &item;
    const auto* ptr = start;
    do {
        result.push_back(to_int(ptr->value()));
        ptr = &ptr->next();
    }
    while (ptr != start);
    return result;
}

} // anonymous namespace


TEST(LinkedListTest, ValueHolder)
{
    auto list = IntList{};
    EXPECT_EQ(values(list), v());

    auto i1 = IntListItem{1};
    auto i2 = IntListItem{2};
    auto i3 = IntListItem{3};

    list.link(i1);
    EXPECT_EQ(values(list), v(1));

    list.link(i2);
    EXPECT_EQ(values(list), v(1, 2));

    list.link(i3);
    EXPECT_EQ(values(list), v(1, 2, 3));

    i2.unlink();
    EXPECT_EQ(values(list), v(1, 3));
}

TEST(LinkedListTest, MovedValueItem)
{
    auto list = IntList{};
    EXPECT_EQ(values(list), v());

    auto i1 = IntListItem{1};
    auto i2 = IntListItem{2};
    auto i3 = IntListItem{3};

    list.link(i1);
    list.link(i2);
    list.link(i3);
    EXPECT_EQ(values(list), v(1, 2, 3));

    EXPECT_EQ(loop(i2), v(2, 3, 0, 1));
    auto i2c = std::move(i2);
    EXPECT_EQ(values(list), v(1, 2, 3));
    EXPECT_EQ(loop(i2), v(2));
    EXPECT_EQ(loop(i2c), v(2, 3, 0, 1));
}

TEST(LinkedListTest, BaseHolder)
{
    auto list = XList{};
    EXPECT_EQ(values(list), v());

    auto i1 = X{ .v = 1 };
    auto i2 = X{ .v = 2 };
    auto i3 = X{ .v = 3 };

    list.link(i1);
    EXPECT_EQ(values(list), v(1));

    list.link(i2);
    EXPECT_EQ(values(list), v(1, 2));

    list.link(i3);
    EXPECT_EQ(values(list), v(1, 2, 3));

    i2.unlink();
    EXPECT_EQ(values(list), v(1, 3));
}

TEST(LinkedListTest, MovedIntrusiveItem)
{
    auto list = XList{};
    EXPECT_EQ(values(list), v());

    auto i1 = X{ .v = 1 };
    auto i2 = X{ .v = 2 };
    auto i3 = X{ .v = 3 };

    list.link(i1);
    list.link(i2);
    list.link(i3);
    EXPECT_EQ(values(list), v(1, 2, 3));

    EXPECT_EQ(loop(i2.linked_list_connectivity()), v(2, 3, 0, 1));
    auto i2c = std::move(i2);
    EXPECT_EQ(values(list), v(1, 2, 3));
    EXPECT_EQ(loop(i2.linked_list_connectivity()), v(2));
    EXPECT_EQ(loop(i2c.linked_list_connectivity()), v(2, 3, 0, 1));
}

TEST(LinkedListTest, AdvancedLinking)
{
    auto list = IntList{};
    EXPECT_EQ(values(list), v());

    auto i1 = IntListItem{1};
    auto i2 = IntListItem{2};
    auto i3 = IntListItem{3};
    auto i4 = IntListItem{4};
    auto i5 = IntListItem{5};

    list.link(i1);
    list.link(i2);
    EXPECT_EQ(values(list), v(1, 2));

    i4.link_to(i3);
    i5.link_to(i3);

    EXPECT_EQ(loop(i3), v(3, 4, 5));

    list.link(i3);
    EXPECT_EQ(values(list), v(1, 2, 3));
    EXPECT_EQ(loop(i4), v(4, 5));

    list.link(i4);
    EXPECT_EQ(values(list), v(1, 2, 3, 4));
    EXPECT_EQ(loop(i5), v(5));

    list.link(i5);
    EXPECT_EQ(values(list), v(1, 2, 3, 4, 5));
}
