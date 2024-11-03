#pragma once

#include "common/func_ref_fwd.hpp"

#include "common/const.hpp"
#include "common/signature.hpp"
#include "common/unsafe.hpp"

#include <utility>


namespace common {

constexpr inline struct Indirect_Tag final {} Indirect;

// ---

template <typename R, typename... A, bool NX>
class FuncRef<R(A...) noexcept(NX)> final
{
public:
    using S = R(A...) noexcept(NX);

    template <typename Context>
    using SX = R(Context*, A...) noexcept(NX);

    struct Storage final
    {
        SX<void>*               f{};
        void*                   c{};
    };

    FuncRef() = default;

    // ---

    template <typename Context>
    FuncRef(SX<Context>* f, Context* c):
        storage_{
            .f{ reinterpret_cast<SX<void>*>(f) },
            .c{ const_cast<void*>(reinterpret_cast<const void*>(c)) } }
    {}

    // ---

    template <S* f>
    /* implicit */ FuncRef(Const_Tag<f>):
        storage_{
            .f{ +[](void*, A... args) noexcept(NX) -> R
                { return f(std::forward<A>(args)...); } },
            .c{ nullptr } }
    {}

    template <typename AcceptedContext,
              SX<AcceptedContext>* f,
              typename PassedContext>
    requires std::convertible_to<PassedContext*, AcceptedContext*>
    FuncRef(Const_Tag<f>, PassedContext* ctx):
        FuncRef{
            +[](AcceptedContext* c, A... args) noexcept(NX) -> R
                { return f(c, std::forward<A>(args)...); },
            static_cast<AcceptedContext*>(ctx) }
    {}

    template <typename Cls, auto f>
    requires MethodWithSignatureOf<decltype(f), Cls, S>
    FuncRef(Const_Tag<f>, Cls* cls):
        FuncRef{
            +[](Cls* cls, A... args) noexcept(NX) -> R
                { return (cls->*f)(std::forward<A>(args)...); },
            cls }
    {}

    // ---

    FuncRef(Indirect_Tag, S* f):
        FuncRef{
            +[](S* f, A... args) noexcept(NX) -> R
                { return f(std::forward<A>(args)...); },
            f }
    {}

    // ---

    template <typename Cls>
    requires std::is_class_v<Cls>
    /* explicit */ FuncRef(Cls* cls):
        FuncRef{
            +[](Cls* cls, A... args) noexcept(NX) -> R
                { return (*cls)(std::forward<A>(args)...); },
            cls }
    {}

    // ---

    auto empty() const noexcept
        -> bool
    { return storage_.f == nullptr; }

    operator bool() const noexcept
    { return !empty(); }

    auto operator()(A... args) const noexcept(NX)
        -> R
    { return storage_.f(storage_.c, std::forward<A>(args)...); }


    // ---

    FuncRef(Unsafe_Tag, SX<void> f, void* c) noexcept
        : storage_{
            .f{ f },
            .c{ c } }
    {}

    auto storage() && noexcept
        -> Storage
    { return std::move(storage_); }

private:

    template<typename Cls>
    struct MethodPtrContext
    {
        Cls* cls;
        R (Cls::*f)(A...) noexcept(NX);
    };

    template<typename Cls>
    struct MethodPtrCapturedContext
    {
        Cls cls;
        R (Cls::*f)(A...) noexcept(NX);
    };

    Storage storage_;
};

// ---

template<SignatureType S>
FuncRef(Indirect_Tag, S* f)
    -> FuncRef<S>;

template<SignatureType S, S* f>
FuncRef(Const_Tag<f>)
    -> FuncRef<S>;

template <typename AcceptedContext, typename R, typename... A, bool NX,
         R(*f)(AcceptedContext*, A...) noexcept(NX),
         typename PassedContext>
requires std::convertible_to<PassedContext*, AcceptedContext*>
FuncRef(Const_Tag<f>, PassedContext* ctx)
    -> FuncRef<R(A...) noexcept(NX)>;

template <typename Cls, auto f>
requires MethodOf<decltype(f), Cls>
FuncRef(Const_Tag<f>, Cls* cls)
    -> FuncRef<SignatureOf_t<decltype(f), Cls>>;

template <typename Cls>
requires std::is_class_v<Cls>
FuncRef(Cls* cls)
    -> FuncRef<SignatureOf_t<decltype(&Cls::operator()), Cls>>;

} // namespace common
