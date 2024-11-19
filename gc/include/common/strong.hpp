#pragma once

#include "common/macro.hpp"
#include "common/zero.hpp"

#include <utility>


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

    auto operator<=>(const Self&) const noexcept
        -> std::strong_ordering = default;
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
