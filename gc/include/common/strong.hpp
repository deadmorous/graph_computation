#pragma once

#include "common/macro.hpp"
#include "common/nil.hpp"
#include "common/strong_fwd.hpp"
#include "common/type.hpp"
#include "common/zero.hpp"

#include <cassert>
#include <iostream>
#include <limits>
#include <type_traits>
#include <utility>


namespace common {

template <typename Traits_>
struct Strong final
{
    using Traits = Traits_;
    using Self = Strong<Traits>;
    using Weak = typename Traits::Weak;
    using View = StrongView<Self>;

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

    Weak v = Traits::default_value();

    constexpr Strong() = default;

    constexpr explicit Strong(Weak v)
        : v{ std::move(v) }
    {}

    template <typename View>
    requires requires{
        typename Traits::Features::View;
        requires std::same_as<std::remove_cvref_t<View>,
                              typename Traits::Features::View>;
    }
    constexpr Strong(View view)
        : v{ view.v }
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

    auto view() const
        -> View
    { return View{ typename View::Weak{v} }; }

    static constexpr auto max() noexcept -> Self
    requires arithmetic
    { return Self{ std::numeric_limits<Weak>::max() }; }

    auto operator+(Self that) const -> Self
        requires is_count
    { return Self{ static_cast<Weak>(v + that.v) }; }

    template <StrongType Index>
        requires is_adjacent_index<Index>
    auto operator+(Index that) const -> Index
    { return Index{ static_cast<Weak>(v + that.v) }; }

    auto operator-(Self that) const -> Self
        requires is_count
    { return Self{ static_cast<Weak>(v - that.v) }; }

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

    auto operator<=>(const View& that) const noexcept
        -> typename Traits::Features::Comparison
    requires (!std::same_as<typename Traits::Features::Comparison, void>) &&
                 HasViewType<typename Traits::Features>
    { return view() <=> that; }

    auto operator==(const Self& that) const noexcept
        -> bool
    requires (!std::same_as<typename Traits::Features::Comparison, void>)
        = default;

    auto operator==(const View& that) const noexcept
        -> bool
    requires (!std::same_as<typename Traits::Features::Comparison, void>) &&
              HasViewType<typename Traits::Features>
    { return view() == that; }
};

// ---

template <typename Weak_, typename Features_>
struct StrongTraits
{
    using Weak = Weak_;
    static constexpr auto default_value() -> Weak
    { return {}; }
    using Features =
        Features_;
};

template <typename Weak_, typename Features_, Weak_ default_value_>
struct StrongWithDefaultTraits
{
    using Weak = Weak_;
    static constexpr auto default_value() -> Weak
    { return default_value_; }
    using Features =
        Features_;
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

template <StrongType View_>
struct StrongStringFeatures final
{
    using StrongDiff = Nil_Tag;
    using View = View_;
    using Comparison = std::strong_ordering;

    static constexpr bool arithmetic = false;
};

// ---

template <StrongType T>
auto operator<<(std::ostream& s, const T& x)
    -> std::ostream&
{ return s << x.v; }

// ---

template <typename T>
requires (!StrongType<T> && !std::is_const_v<T>)
constexpr auto raw(T& x) noexcept -> T&
{ return x; }

template <typename T>
requires (!StrongType<T>)
constexpr auto raw(const T& x) noexcept -> const T&
{ return x; }

template <StrongType T>
constexpr auto raw(T& x) noexcept -> typename T::Weak&
{ return x.v; }

template <StrongType T>
constexpr auto raw(const T& x) noexcept -> const typename T::Weak&
{ return x.v; }

} // namespace common


#define GCLIB_STRONG_TYPE(Name, Weak_, ...)                                 \
    struct Name##_StrongTraits final :                                      \
        common::StrongTraits<                                               \
            Weak_,                                                          \
            GCLIB_DEFAULT_A0_TO(::common::StrongIdFeatures, ##__VA_ARGS__)> \
    {};                                                                     \
    using Name = ::common::Strong<Name##_StrongTraits>

#define GCLIB_STRONG_TYPE_WITH_DEFAULT(Name, Weak_, Default, ...)           \
    struct Name##_StrongTraits final :                                      \
        common::StrongWithDefaultTraits<                                    \
            Weak_,                                                          \
            GCLIB_DEFAULT_A0_TO(::common::StrongIdFeatures, ##__VA_ARGS__), \
            Default>                                                        \
    {};                                                                     \
    using Name = ::common::Strong<Name##_StrongTraits>

#define GCLIB_STRONG_LITERAL_SUFFIX(Name, suffix)                           \
    inline Name operator"" suffix(unsigned long long value)                 \
    {                                                                       \
        assert(value <= std::numeric_limits<Name::Weak>::max());            \
        return Name{static_cast<Name::Weak>(value)};                        \
    }                                                                       \
    static_assert(true)

#define GCLIB_STRONG_STRING_VIEW(Name)                                      \
    GCLIB_STRONG_TYPE(Name, std::string_view)

#define GCLIB_STRONG_STRING(Name)                                           \
    GCLIB_STRONG_STRING_VIEW(Name##View);                                   \
    GCLIB_STRONG_TYPE(Name, std::string,                                    \
                      common::StrongStringFeatures<Name##View>)
