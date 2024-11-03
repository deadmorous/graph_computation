#pragma once

namespace common {

template <typename S>
constexpr inline auto is_signature_type = false;

template <typename R, typename... Args, bool NX>
constexpr inline auto
    is_signature_type<R(Args...) noexcept(NX)> = true;

template <typename S>
concept SignatureType = is_signature_type< S >;

} // namespace common
