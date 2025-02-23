#include "common/strong.hpp"
#include "common/index_range.hpp"
#include "common/strong_grouped.hpp"
#include "common/strong_span.hpp"
#include "common/strong_vector.hpp"

#include <gtest/gtest.h>
#include <unordered_set>

#include "common/detail/hash.hpp"


using namespace std::literals;

namespace {

GCLIB_STRONG_TYPE_WITH_DEFAULT(IntCountDefaultedTo33,
                               int,
                               33,
                               common::StrongCountFeatures);

GCLIB_STRONG_TYPE(Count, ptrdiff_t, common::StrongCountFeatures);
GCLIB_STRONG_TYPE(Index, ptrdiff_t, common::StrongIndexFeatures<Count>);

GCLIB_STRONG_TYPE(Count2, ptrdiff_t, common::StrongCountFeatures);
GCLIB_STRONG_TYPE(Index2, ptrdiff_t, common::StrongIndexFeatures<Count2>);

template <typename Lhs, typename Rhs>
concept CanAdd = requires(Lhs lhs, Rhs rhs)
{ lhs + rhs; };

template <typename Lhs, typename Rhs>
concept CanSub = requires(Lhs lhs, Rhs rhs)
{ lhs - rhs; };

// GCLIB_STRONG_TYPE(StrV, std::string_view);
// GCLIB_STRONG_TYPE(Str, std::string, common::StrongStringFeatures<StrV>);
GCLIB_STRONG_STRING(Str);

} // anonymous namespace

TEST(Common_Strong, Basic)
{
    using X = IntCountDefaultedTo33;
    X x;
    EXPECT_EQ(x.v, 33);
    static_assert(X::arithmetic);
    static_assert(X::is_count);
    static_assert(!X::is_index);

    X x1 = common::Zero;
    EXPECT_EQ(x1.v, 0);

    X x2{1};
    EXPECT_EQ(x2.v, 1);

    auto sum = x + x2;
    static_assert(std::same_as<decltype(sum), X>);
    EXPECT_EQ(sum.v, 34);

    auto diff = x - x2;
    static_assert(std::same_as<decltype(sum), X>);
    EXPECT_EQ(diff.v, 32);
}

TEST(Common_Strong, Arithmetic)
{
    auto i0 = Index{};
    auto i1 = Index{1};
    auto i2 = Index{5};
    auto c0 = Count{3};
    auto c1 = Count{4};

    EXPECT_EQ(i0.v, 0);
    EXPECT_EQ(i1.v, 1);
    EXPECT_EQ(i2.v, 5);
    EXPECT_EQ(c0.v, 3);
    EXPECT_EQ(c1.v, 4);

    static_assert(!CanAdd<Index, Index>);

    static_assert(CanAdd<Index, Count>);
    auto index_plus_count = i2 + c0;
    static_assert(std::same_as<decltype(index_plus_count), Index>);
    EXPECT_EQ(index_plus_count.v, 8);

    static_assert(CanAdd<Count, Index>);
    auto count_plus_index = c0 + i2;
    static_assert(std::same_as<decltype(count_plus_index), Index>);
    EXPECT_EQ(count_plus_index.v, 8);

    static_assert(CanAdd<Count, Count>);
    auto count_plus_count = c0 + c1;
    static_assert(std::same_as<decltype(count_plus_count), Count>);
    EXPECT_EQ(count_plus_count.v, 7);



    static_assert(CanSub<Index, Index>);
    auto index_minus_index = i2 - i1;
    static_assert(std::same_as<decltype(index_minus_index), Count>);
    EXPECT_EQ(index_minus_index.v, 4);

    static_assert(CanSub<Index, Count>);
    auto index_minus_count = i2 - c0;
    static_assert(std::same_as<decltype(index_minus_count), Index>);
    EXPECT_EQ(index_minus_count.v, 2);

    static_assert(!CanSub<Count, Index>);

    static_assert(CanSub<Count, Count>);
    auto count_minus_count = c0 - c1;
    static_assert(std::same_as<decltype(count_minus_count), Count>);
    EXPECT_EQ(count_minus_count.v, -1);

    EXPECT_EQ(i1++, Index{1});
    EXPECT_EQ(i1, Index{2});
    EXPECT_EQ(++i1, Index{3});
    EXPECT_EQ(i1, Index{3});
    EXPECT_EQ(i1++, Index{3});
    EXPECT_EQ(i1, Index{4});

    EXPECT_EQ(i1--, Index{4});
    EXPECT_EQ(i1, Index{3});
    EXPECT_EQ(--i1, Index{2});
    EXPECT_EQ(i1, Index{2});
    EXPECT_EQ(i1--, Index{2});
    EXPECT_EQ(i1, Index{1});

    i1 += c0;
    EXPECT_EQ(i1, Index{4});
    i1 -= c0;
    EXPECT_EQ(i1, Index{1});

    c0 += c1;
    EXPECT_EQ(c0, Count{7});
    c0 -= c1;
    EXPECT_EQ(c0, Count{3});
}

TEST(Common_Strong, Ranges)
{
    auto i1 = Index{5};
    auto i2 = Index{10};
    auto c = Count{10};

    auto test_range =
        []<typename R, typename I>(
            R range, common::Type_Tag<I>, size_t begin, size_t size)
    {
        auto has_first = false;
        auto iter_count = size_t{};
        I first;
        for (auto i : range)
        {
            static_assert(std::same_as<I, decltype(i)>);
            if (!has_first)
            {
                first = i;
                has_first = true;
            }
            EXPECT_EQ(i, first+typename R::Count(iter_count));
            ++iter_count;
        }
        EXPECT_EQ(typename R::Count(iter_count), range.size());
        EXPECT_EQ(iter_count, size);
    };

    constexpr auto index_tag = common::Type<Index>;
    constexpr auto int_tag = common::Type<int>;
    test_range(common::index_range(i1, i2), index_tag, i1.v, (i2-i1).v);
    test_range(common::index_range(i1, c), index_tag, i1.v, c.v);
    test_range(common::sized_index_range(i1, c), index_tag, i1.v, c.v);
    test_range(common::index_range<Index>(c), index_tag, 0, c.v);
    test_range(common::index_range(5, 10), int_tag, 5, 10-5);
    test_range(common::sized_index_range(5, 10), int_tag, 5, 10);
    test_range(common::index_range(10), int_tag, 0, 10);
}

TEST(Common_Strong, Vector)
{
    using V = common::StrongVector<std::string, Index>;

    V v(Count(3));
    v[common::Zero] = "one";
    v.at(Index{1}) = "two";
    v.at(Index{2}) = "three";
    v.reserve(Count{11});
    EXPECT_EQ(v.capacity(), Count{11});
    v.resize(Count{6}, "many");
    v.resize(Count{10});
    v.back() = "a lot";
    v.push_back("a lot + 1");
    EXPECT_EQ(v.size(), Count{11});

    auto expected = std::initializer_list<std::string_view>{
        "one"sv,
        "two"sv,
        "three"sv,
        "many"sv,
        "many"sv,
        "many"sv,
        ""sv,
        ""sv,
        ""sv,
        "a lot"sv,
        "a lot + 1"sv };
    auto expected_it = expected.begin();
    for (const auto& s : v)
    {
        EXPECT_EQ(s, *expected_it);
        ++expected_it;
    }

    expected_it = expected.begin();
    for (auto i : v.index_range())
    {
        EXPECT_EQ(v[i], *expected_it);
        EXPECT_EQ(v.at(i), *expected_it);
        ++expected_it;
    }

    v.pop_back();
    EXPECT_EQ(v.size(), Count{10});

    EXPECT_THROW(v.at(Index{11}), std::out_of_range);

    auto v1 = v;
    EXPECT_EQ(v1, v);
    EXPECT_FALSE(v1 < v);
    EXPECT_FALSE(v1 != v);
    EXPECT_FALSE(v1 > v);
    EXPECT_TRUE(v1 <= v);
    EXPECT_TRUE(v1 >= v);
}

TEST(Common_Strong, Span)
{
    auto check_span =
        []<typename V, typename I, std::size_t E>(
            common::StrongSpan<V, I, E> span,
            std::initializer_list<V> contents)
    {
        using C = typename I::StrongDiff;
        EXPECT_EQ(span.size(), C(contents.size()));
        EXPECT_EQ(span.empty(), span.size() == common::Zero);
        auto it = contents.begin();
        for (I i : span.index_range())
        {
            EXPECT_EQ(span[i], *it);
            ++it;
        }

        it = contents.begin();
        for (const auto& v : span)
            EXPECT_EQ(v, *it++);
        };

    {
        using V = common::StrongVector<std::string, Index>;
        using W = V::Weak;
        auto v = V{ W{ "one", "two", "three", "four" } };
        using S = common::StrongSpan<std::string, Index>;

        auto s = S{v};
        check_span(s, { "one"s, "two"s, "three"s, "four"s });
        check_span(s.subspan(Count{2}), {"one"s, "two"s});
        check_span(s.subspan(Index{2}), {"three"s, "four"s});
        check_span(s.subspan(Index{1}, Count{2}), {"two"s, "three"s});
        check_span(s.subspan(Index{1}, Index{2}), {"two"s});
        check_span(s.subspan(Index{1}, Index{1}), {});
    }

    {
        int n3[] = {123, 45, 67};
        using S3 = common::StrongSpan<int, Index, 3>;
        using S = common::StrongSpan<int, Index>;

        auto s3 = S3{n3};
        auto s = S{n3};

        check_span(s3, {123, 45, 67});
        check_span(s, {123, 45, 67});

        check_span(s3.subspan(Index{1}), {45, 67});
        check_span(s.subspan(Index{1}), {45, 67});
    }
}

TEST(Common_Strong, Grouped)
{
    using G = common::StrongGrouped<int, Index, Index2>;

    auto g = G{};
    add_to_last_group(g, 1);
    add_to_last_group(g, 2);
    add_to_last_group(g, 3);
    next_group(g);
    add_to_last_group(g, 10);
    add_to_last_group(g, 20);
    next_group(g);
    add_to_last_group(g, 100);
    add_to_last_group(g, 200);
    add_to_last_group(g, 300);
    add_to_last_group(g, 400);
    next_group(g);
    next_group(g);
    add_to_last_group(g, 10'000);
    next_group(g);

    EXPECT_EQ(common::format(g),
              "[(1,2,3), (10,20), (100,200,300,400), (), (10000)]");
    EXPECT_EQ(group_count(g), Count{5});

    EXPECT_EQ(group_indices(g), common::index_range<Index>(Count{5}));
    EXPECT_EQ(group_indices(g.v), common::index_range(uint32_t{5}));

    auto g0 = group(g, Index{0});
    EXPECT_EQ(g0.size(), Count2{3});
    EXPECT_EQ(g0[Index2{0}], 1);
    EXPECT_EQ(g0[Index2{1}], 2);
    EXPECT_EQ(g0[Index2{2}], 3);
    auto g4 = group(g, Index{3});
    EXPECT_TRUE(g4.empty());
    auto g5 = group(g, Index{4});
    EXPECT_EQ(g5.size(), Count2{1});
    EXPECT_EQ(g5[Index2{0}], 10'000);

    g0[Index2{1}] = 8;

    const auto& cg = g;
    auto cg0 = group(cg, Index{0});
    EXPECT_EQ(cg0.size(), Count2{3});
    EXPECT_EQ(cg0[Index2{0}], 1);
    EXPECT_EQ(cg0[Index2{1}], 8);
    EXPECT_EQ(cg0[Index2{2}], 3);
}

TEST(Common_Strong, String)
{
    static_assert(std::same_as<StrView, Str::View>);

    auto strv = StrView{"asd"};
    EXPECT_EQ(strv.v, common::format(strv));
    auto s1 = std::string{strv.v};

    auto str = Str{"qwe"};
    EXPECT_EQ(str.v, common::format(str));
    auto s2 = std::string{str.v};

    auto str_from_v = Str{ strv };
    static_assert(std::same_as<decltype(str_from_v), Str>);

    auto v_from_str = str_from_v.view();
    ASSERT_EQ(v_from_str, strv);
    static_assert(std::same_as<decltype(v_from_str), StrView>);

    // All these should not create a string from a view (TODO: Check it)
    [[maybe_unused]] auto s_v = str <=> v_from_str;
    [[maybe_unused]] auto v_s = v_from_str <=> str;
    [[maybe_unused]] auto s_eq_v = str == v_from_str;
    [[maybe_unused]] auto v_eq_s = v_from_str == str;
    [[maybe_unused]] auto s_neq_v = str != v_from_str;
    [[maybe_unused]] auto v_neq_s = v_from_str != str;

    EXPECT_FALSE(v_from_str == str);
    EXPECT_FALSE(str == v_from_str);
    EXPECT_TRUE(v_from_str == strv);

    using S = std::unordered_set<StrView, common::detail::Hash>;
    S s;
    s.emplace(strv);
    s.emplace(StrView{"wow"});
    s.emplace(str.view());
    EXPECT_TRUE(s.contains(Str::View{"wow"}));
    EXPECT_TRUE(s.contains(strv));
    EXPECT_TRUE(s.contains(str.view()));
    EXPECT_FALSE(s.contains(StrView{"one"}));
}
