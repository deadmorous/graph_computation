#pragma once

#include "common/grouped.hpp"
#include "common/index_range.hpp"
#include "common/strong_fwd.hpp"
#include "common/strong_span.hpp"

#include <cassert>
#include <limits>

namespace common {

template <typename T, StrongIndexType OI, StrongIndexType II>
struct StrongGrouped
{
    using OuterIndex = OI;
    using OuterCount = typename OI::StrongDiff;
    using InnerIndex = II;
    using InnerCount = typename II::StrongDiff;
    using Weak = Grouped<T>;
    using value_type = T;

    Weak v;
};

template <typename>
constexpr inline auto is_strong_grouped_v = false;

template <typename T, StrongIndexType OI, StrongIndexType II>
constexpr inline auto is_strong_grouped_v<StrongGrouped<T, OI, II>> = true;

template <typename T>
concept StrongGroupedType =
    is_strong_grouped_v<T>;

template <StrongGroupedType SG, typename V>
requires std::convertible_to<V, typename SG::value_type>
auto add_to_last_group(SG& grouped, V&& value)
    -> void
{ add_to_last_group(grouped.v, std::forward<V>(value)); }

template <StrongGroupedType SG>
auto next_group(SG& grouped)
    -> void
{ next_group(grouped.v); }

template <StrongGroupedType SG>
auto group_count(const SG& grouped)
    -> typename SG::OuterCount
{
    auto raw = group_count(grouped.v);
    assert(raw <= std::numeric_limits<typename SG::OuterCount::Weak>::max());
    return typename SG::OuterCount(raw);
}

template <StrongGroupedType SG>
auto group_indices(const SG& grouped)
    -> IndexRange<typename SG::OuterIndex>
{
    using OuterIndex = typename SG::OuterIndex;
    using OuterCount = typename SG::OuterCount;
    auto raw = group_count(grouped.v);
    assert(raw <= std::numeric_limits<typename OuterCount::Weak>::max());
    return index_range<OuterIndex>(OuterCount(raw));
}

template <StrongGroupedType SG>
auto group(const SG& grouped, typename SG::OuterIndex igroup)
    -> StrongSpan<const typename SG::value_type, typename SG::InnerIndex>
{
    return StrongSpan<const typename SG::value_type, typename SG::InnerIndex>(
        group(grouped.v, igroup.v) );
}

template <StrongGroupedType SG>
auto group(SG& grouped, typename SG::OuterIndex igroup)
    -> StrongSpan<typename SG::value_type, typename SG::InnerIndex>
{
    return StrongSpan<typename SG::value_type, typename SG::InnerIndex>(
        group(grouped.v, igroup.v) );
}

template <StrongGroupedType SG>
auto operator<<(std::ostream& s, const SG& grouped)
    -> std::ostream&
{ return s << grouped.v; }

} // namespace common
