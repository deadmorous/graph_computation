#pragma once

#include <concepts>


namespace common {

template <typename Traits_>
struct Strong;

template <typename T>
constexpr inline auto is_strong_v = false;

template <typename Traits>
constexpr inline auto is_strong_v<Strong<Traits>> = true;

template <typename T>
concept StrongType =
    is_strong_v<T>;

template <typename T>
concept StrongArithmeticType =
    StrongType<T> && T::arithmetic;

template <typename T>
concept StrongCountType =
    StrongType<T> && T::is_count;

template <typename T>
concept StrongIndexType =
    StrongType<T> &&
    T::is_index &&
    StrongCountType< typename T::StrongDiff >;

} // namespace common
