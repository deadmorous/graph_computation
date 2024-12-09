#pragma once

#include "common/macro.hpp"
#include "common/nil.hpp"
#include "common/strong_fwd.hpp"
#include "common/type.hpp"
#include "common/zero.hpp"

#include <cassert>
#include <iostream>
#include <limits>
#include <utility>


namespace common {

template <typename Traits_>
struct Strong final
{
    using Traits = Traits_;
    using Self = Strong<Traits>;
    using Weak = typename Traits::Weak;

    using StrongDiff = typename Traits::Features::StrongDiff;
    using Comparison = typename Traits::Features::Comparison;

    static constexpr auto arithmetic = Traits::Features::arithmetic;

    static constexpr auto is_count =
        arithmetic && std::same_as<StrongDiff, Nil_Tag>;

    static constexpr auto is_index =
        arithmetic && !std::same_as<StrongDiff, Nil_Tag>;

    template <StrongType Index>
    static constexpr bool is_adjacent_index =
        is_count &&
        Index::is_index &&
        std::same_as<typename Index::StrongDiff, Self>;

    Weak v = Traits::default_value;

    constexpr Strong() = default;

    constexpr explicit Strong(Weak v)
        : v{ std::move(v) }
    {}

    constexpr /* implicit */ Strong(Zero_Tag)
    requires std::default_initializable<Weak>
        : v{}
    {}

    Strong(const Strong&) = default;
    Strong(Strong&&) = default;
    auto operator=(const Strong&) -> Strong& = default;
    auto operator=(Strong&&) -> Strong& = default;

    auto operator=(Zero_Tag) -> Strong&
    requires std::assignable_from<Weak&, const Weak&>
    {
        v = Weak{};
        return *this;
    }

    auto operator+(Self that) const -> Self
        requires is_count
    { return Self{ v + that.v }; }

    template <StrongType Index>
        requires is_adjacent_index<Index>
    auto operator+(Index that) const -> Index
    { return Index{ v + that.v }; }

    auto operator-(Self that) const -> Self
        requires is_count
    { return Self{ v - that.v }; }

    auto operator+=(Self that) -> Self&
        requires is_count
    {
        v += that.v;
        return *this;
    }

    auto operator-=(Self that) -> Self&
        requires is_count
    {
        v -= that.v;
        return *this;
    }

    auto operator+(StrongDiff that) const -> Self
        requires is_index
    { return Self{ static_cast<Weak>(v + that.v) }; }

    auto operator-(StrongDiff that) const -> Self
        requires is_index
    { return Self{ static_cast<Weak>(v - that.v) }; }

    auto operator-(Self that) const -> StrongDiff
        requires is_index
    { return StrongDiff{ static_cast<Weak>(v - that.v) }; }

    auto operator+=(StrongDiff that) -> Self&
        requires is_index
    {
        v += that.v;
        return *this;
    }

    auto operator-=(StrongDiff that) -> Self&
        requires is_index
    {
        v -= that.v;
        return *this;
    }

    auto operator++() -> Self
        requires arithmetic
    {
        ++v;
        return *this;
    }

    auto operator++(int) -> Self
        requires arithmetic
    {
        auto result = *this;
        ++v;
        return result;
    }

    auto operator--() -> Self
        requires arithmetic
    {
        --v;
        return *this;
    }

    auto operator--(int) -> Self
        requires arithmetic
    {
        auto result = *this;
        --v;
        return result;
    }

    auto operator<=>(const Self&) const noexcept
        -> typename Traits::Features::Comparison
    requires (!std::same_as<typename Traits::Features::Comparison, void>)
        = default;
};

// ---

struct StrongIdFeatures final
{
    using StrongDiff = Nil_Tag;
    using Comparison = std::strong_ordering;

    static constexpr bool arithmetic = false;
};

struct StrongCountFeatures final
{
    using StrongDiff = Nil_Tag;
    using Comparison = std::strong_ordering;

    static constexpr bool arithmetic = true;
};

template <StrongType Count>
struct StrongIndexFeatures final
{
    using StrongDiff = Count;
    using Comparison = std::strong_ordering;

    static constexpr bool arithmetic = true;
};

// ---

template <StrongType T>
auto operator<<(std::ostream& s, const T& x)
    -> std::ostream&
{ return s << x.v; }

// ---

template <typename T>
requires (!StrongType<T>)
constexpr auto raw(T& x) noexcept -> T&
{ return x; }

template <StrongType T>
constexpr auto raw(T& x) noexcept -> typename T::Weak&
{ return x.v; }

template <StrongType T>
constexpr auto raw(const T& x) noexcept -> const typename T::Weak&
{ return x.v; }

} // namespace common


#define GCLIB_STRONG_TYPE(Name, Weak_, ...)                                 \
    struct Name##_StrongTraits final                                        \
    {                                                                       \
        using Weak = Weak_;                                                 \
        static constexpr Weak default_value =                               \
            GCLIB_DEFAULT_A0_TO({}, ##__VA_ARGS__);                         \
        using Features =                                                    \
            GCLIB_DEFAULT_A1_TO(::common::StrongIdFeatures, ##__VA_ARGS__); \
    };                                                                      \
    using Name = ::common::Strong<Name##_StrongTraits>

#define GCLIB_STRONG_LITERAL_SUFFIX(Name, suffix)                           \
    inline Name operator"" suffix(unsigned long long value)                 \
    {                                                                       \
        assert(value <= std::numeric_limits<Name::Weak>::max());            \
        return Name{static_cast<Name::Weak>(value)};                        \
    }                                                                       \
    static_assert(true)
