#pragma once

#include "common/strong_fwd.hpp"
#include "common/struct_type.hpp"

#include <concepts>
#include <cstddef>
#include <string>
#include <utility>
#include <vector>   // note: one of headers declaring `std::hash`

namespace common::detail {

template <common::StrongType T>
auto hash(const T& v) noexcept
    -> size_t
{ return std::hash<typename T::Weak>{}(v.v); }

template <std::integral T>
auto hash(const T& v) noexcept
    -> size_t
{ return std::hash<T>{}(v); }

inline auto hash(const std::string& v) noexcept
    -> size_t
{ return std::hash<std::string>{}(v); }

template<size_t... I, typename... Ts>
auto hash_impl(std::index_sequence<I...>, const Ts&... vs) noexcept
    -> size_t
{ return ((hash(vs) << I) ^ ...); }

template<size_t... I, typename... Ts>
auto tuple_hash_impl(std::index_sequence<I...> iseq,
                     const std::tuple<Ts...>& v) noexcept
    -> size_t
{ return hash_impl(iseq, std::get<I>(v)...); }

template<typename T0, typename T1, typename... Ts>
auto hash(const T0& v0, const T1& v1, const Ts&... vs) noexcept
    -> size_t
{ return hash_impl(std::index_sequence_for<T0, T1, Ts...>(), v0, v1, vs...); }

template <typename... Ts>
auto hash(const std::tuple<Ts...>& v) noexcept
    -> size_t
{ return tuple_hash_impl(std::index_sequence_for<Ts...>(), v); }

template <typename... Ts>
auto hash(const std::pair<Ts...>& v) noexcept
    -> size_t
{ return hash(v.first, v.second); }

template <StructType T>
auto hash(const T& v) noexcept
    -> size_t
{ return hash(fields_of(v)); }

// TODO: Generalize for a wider category of sequences
template <typename T>
auto hash(const std::vector<T>& v) noexcept
    -> size_t
{
    auto result = size_t{};
    for (const auto& element : v)
        result = (result << 1) ^ hash(element);
    return result;
}

struct Hash final
{
    template <typename T>
    auto operator()(const T& v) const noexcept
        -> size_t
    { return hash(v); }
};

} // namespace common::detail
