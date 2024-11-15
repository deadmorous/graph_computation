#pragma once

#include "common/macro.hpp"


namespace common {

template <typename Traits_>
struct Strong final
{
    using Traits = Traits_;
    using Self = Strong<Traits>;
    using Weak = typename Traits::Weak;
    Weak v = Traits::default_value();

    constexpr Strong() = default;

    constexpr explicit Strong(Weak v)
        : v{ v }
    {}

    auto operator<=>(const Self&) const noexcept = default;
};

template <typename T>
struct StrongTraits
{
    using Weak = T;
};

// ---

template <typename T>
constexpr inline auto is_strong_v = false;

template <typename Traits>
constexpr inline auto is_strong_v<Strong<Traits>> = true;

template <typename T>
concept StrongType =
    is_strong_v<T>;

} // namespace common


#define GCLIB_STRONG_TYPE(Name, Weak, ...)                                  \
    struct Name##_StrongTraits : ::common::StrongTraits<Weak> {             \
        static auto default_value() noexcept -> Weak                        \
        { return GCLIB_DEFAULT_TO({}, ##__VA_ARGS__); } };                  \
    using Name = ::common::Strong<Name##_StrongTraits>
