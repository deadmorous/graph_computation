#pragma once

#include "common/signature_fwd.hpp"

#include "common/type.hpp"

#include <type_traits>


namespace common {

template <typename M, typename Cls>
struct SignatureOf;

template <typename M, typename Cls>
using SignatureOf_t = typename SignatureOf<M, Cls>::type;

template <typename R, typename... A, bool NX>
struct SignatureOf<R(*)(A...) noexcept(NX), void>
{ using type = R(A...) noexcept(NX); };

template <typename R, typename... A, bool NX, typename Cls>
struct SignatureOf<R(Cls::*)(A...) noexcept(NX), Cls>
{ using type = R(A...) noexcept(NX); };

template <typename R, typename... A, bool NX, typename Cls>
struct SignatureOf<R(Cls::*)(A...) & noexcept(NX), Cls>
{ using type = R(A...) noexcept(NX); };

template <typename R, typename... A, bool NX, typename Cls>
struct SignatureOf<R(std::remove_const_t<Cls>::*)(A...) const noexcept(NX), Cls>
{ using type = R(A...) noexcept(NX); };

template <typename R, typename... A, bool NX, typename Cls>
struct SignatureOf<R(std::remove_const_t<Cls>::*)(A...) const & noexcept(NX), Cls>
{ using type = R(A...) noexcept(NX); };

// ---


template <typename M, typename Cls>
constexpr inline auto is_method_of = false;

template <typename R, typename... A, bool NX, typename Cls>
constexpr inline auto is_method_of<R(Cls::*)(A...) noexcept(NX), Cls> = true;

template <typename R, typename... A, bool NX, typename Cls>
constexpr inline auto is_method_of<R(Cls::*)(A...) & noexcept(NX), Cls> = true;

template <typename R, typename... A, bool NX, typename Cls>
constexpr inline auto is_method_of<
    R(std::remove_const_t<Cls>::*)(A...) const noexcept(NX), Cls> = true;

template <typename R, typename... A, bool NX, typename Cls>
constexpr inline auto is_method_of<
    R(std::remove_const_t<Cls>::*)(A...) const & noexcept(NX), Cls> = true;

template <typename M, typename Cls>
concept MethodOf = is_method_of<M, Cls>;

// ---

template <typename M, typename Cls, typename S>
constexpr inline auto is_method_with_signature_of = false;

template <typename R, typename... A, bool NX, typename Cls>
constexpr inline auto is_method_with_signature_of<
    R(Cls::*)(A...) noexcept(NX), Cls, R(A...) noexcept(NX)> = true;

template <typename R, typename... A, bool NX, typename Cls>
constexpr inline auto is_method_with_signature_of<
    R(Cls::*)(A...) & noexcept(NX), Cls, R(A...) noexcept(NX)> = true;

template <typename R, typename... A, bool NX, typename Cls>
constexpr inline auto is_method_with_signature_of<
    R(std::remove_const_t<Cls>::*)(A...) const noexcept(NX),
    Cls,
    R(A...) noexcept(NX)> = true;

template <typename R, typename... A, bool NX, typename Cls>
constexpr inline auto is_method_with_signature_of<
    R(std::remove_const_t<Cls>::*)(A...) const & noexcept(NX),
    Cls,
    R(A...) noexcept(NX)> = true;

template <typename M, typename Cls, typename S>
concept MethodWithSignatureOf = is_method_with_signature_of<M, Cls, S>;

// ---

template <SignatureType S>
constexpr auto signature(S* f) noexcept
    -> Type_Tag<S>
{ return {}; }

template <typename R, typename... A, bool NX, typename Cls>
constexpr auto signature(R(Cls::*)(A...) noexcept(NX)) noexcept
    -> Type_Tag<R(A...) noexcept(NX)>
{ return {}; }

template <typename R, typename... A, bool NX, typename Cls>
constexpr auto signature(R(Cls::*)(A...) & noexcept(NX)) noexcept
    -> Type_Tag<R(A...) noexcept(NX)>
{ return {}; }

template <typename R, typename... A, bool NX, typename Cls>
constexpr auto signature(
    R(Cls::*)(A...) const noexcept(NX)) noexcept
        -> Type_Tag<R(A...) noexcept(NX)>
{ return {}; }

template <typename R, typename... A, bool NX, typename Cls>
constexpr auto signature(
    R(Cls::*)(A...) const & noexcept(NX)) noexcept
    -> Type_Tag<R(A...) noexcept(NX)>
{ return {}; }

// ---

template <typename R, typename... A, bool NX>
constexpr auto return_type(Type_Tag<R(A...) noexcept(NX)>)
    -> Type_Tag<R>
{ return {}; }

template <typename>
struct ReturnType;

template <typename T>
using ReturnType_t = typename ReturnType<T>::type;

template <typename R, typename... A, bool NX>
struct ReturnType<R(A...) noexcept(NX)>
{ using type = R; };

} // namespace common
