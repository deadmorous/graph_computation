#pragma once


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

} // namespace common
