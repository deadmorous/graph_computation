#pragma once

#include "common/format.hpp"
#include "common/index_range.hpp"

#include <cassert>
#include <cstdint>
#include <span>
#include <vector>


namespace common {

template <typename T>
struct Grouped
{
    using value_type = T;

    std::vector<T>          values;
    std::vector<uint32_t>   groups{0};

    auto operator==(const Grouped&) const noexcept -> bool = default;
};

template <typename T, typename V>
requires std::convertible_to<V, T>
auto add_to_last_group(Grouped<T>& grouped, V&& value)
    -> void
{ grouped.values.emplace_back(std::forward<V>(value)); }

template <typename T>
auto next_group(Grouped<T>& grouped)
    -> void
{ grouped.groups.push_back(grouped.values.size()); }

template <typename T>
auto group_count(const Grouped<T>& grouped)
    -> uint32_t
{
    assert(!grouped.groups.empty());
    return grouped.groups.size() - 1;
}

template <typename T>
auto group_indices(const Grouped<T>& grouped)
    -> IndexRange<uint32_t>
{ return index_range(group_count(grouped)); }

template <typename T>
auto group(const Grouped<T>& grouped, uint32_t igroup)
    -> std::span<const T>
{
    assert(igroup + 1 < grouped.groups.size());
    const auto* indices = grouped.groups.data() + igroup;
    const auto* data = grouped.values.data();
    return { data+indices[0], data+indices[1] };
}

template <typename T>
auto group(Grouped<T>& grouped, uint32_t igroup)
    -> std::span<T>
{
    assert(igroup + 1 < grouped.groups.size());
    auto* indices = grouped.groups.data() + igroup;
    auto* data = grouped.values.data();
    return { data+indices[0], data+indices[1] };
}

template <typename T>
auto operator<<(std::ostream& s, const Grouped<T>& grouped)
    -> std::ostream&
{
    std::string_view delim = "";
    s << '[';
    for (uint32_t ig=0, ng=group_count(grouped); ig<ng; ++ig)
    {
        auto g = group(grouped, ig);
        s << delim << '(' << format_seq(g) << ')';
        delim = ", ";
    }
    s << ']';
    return s;
}

} // namespace common
