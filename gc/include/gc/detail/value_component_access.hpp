#pragma once

#include "gc/type_fwd.hpp"
#include "gc/value_path.hpp"

#include "common/maybe_const.hpp"
#include "common/strong.hpp"
#include "common/throw.hpp"
#include "common/tuple_like.hpp"

#include <any>
#include <cassert>
#include <functional>
#include <memory>
#include <stdexcept>
#include <tuple>


namespace gc::detail {

template <typename T>
constexpr inline auto actually_equality_comparable_v =
    std::equality_comparable<T>;

template <typename T>
    requires requires(const T& v)
    { std::begin(v); }
constexpr inline auto actually_equality_comparable_v<T> =
    actually_equality_comparable_v<decltype(*std::begin(std::declval<T>()))>;

template <typename... Ts>
constexpr inline auto actually_equality_comparable_v<std::tuple<Ts...>> =
    (actually_equality_comparable_v<Ts> && ...);

template <typename T0, typename T1>
constexpr inline auto actually_equality_comparable_v<std::pair<T0, T1>> =
    actually_equality_comparable_v<T0> && actually_equality_comparable_v<T1>;

// NOTE: The reason to introduce `actually_equality_comparable` is that
// `std::equality_comparable` is always `true` for standard containers, which
// leads to compilation errors when comparing a container of `T` elements with
// `std::equality_comparable<T>` == false.
template <typename T>
concept actually_equality_comparable = actually_equality_comparable_v<T>;

// ---

template <typename Type>
struct ValueComponentAccess
{
    virtual ~ValueComponentAccess() = default;

    virtual auto keys(const std::any& data) const
        -> std::vector<ValuePathItem> = 0;

    virtual auto get(ValuePathView path,
                     const std::any& data) const
        -> std::pair<const Type*, std::any> = 0;

    virtual auto set(ValuePathView path,
                     std::any& data,
                     const std::any& src) const
        -> void = 0;

    virtual auto size(ValuePathView path,
                      const std::any& data) const
        -> size_t = 0;

    virtual auto resize(ValuePathView path,
                        std::any& data,
                        size_t size) const
        -> void = 0;

    virtual auto equal(const std::any& lhs, const std::any& rhs) const
        -> bool = 0;

    virtual auto make_data() const
        -> std::any = 0;
};

// ---

template <typename T>
auto unpack(std::any& data, common::Type_Tag<T> = {})
    -> T&
{ return std::any_cast<T&>(data); }

template <typename T>
auto unpack(const std::any& data, common::Type_Tag<T> = {})
    -> const T&
{ return std::any_cast<const T&>(data); }

template <size_t N, typename F, typename... Args>
auto visit_index(std::integral_constant<size_t, N>,
                 size_t index,
                 F&& f,
                 Args&&... args)
    -> std::invoke_result_t< F, std::integral_constant<size_t, 0>, Args... >
{
    if constexpr(N == 0)
        throw std::invalid_argument("Index is out of range");

    else
    {
        constexpr auto prev = std::integral_constant<size_t, N-1>{};
        if (index + 1 == N)
            return std::invoke(std::forward<F>(f),
                               prev,
                               std::forward<Args>(args)...);
        else
            return visit_index(prev,
                               index,
                               std::forward<F>(f),
                               std::forward<Args>(args)...);
    }
}

// ---

template <typename Type, typename T>
struct ValueComponents;

template <typename Type, typename T>
struct ValueComponentAccessImpl final : ValueComponentAccess<Type>
{
    auto keys(const std::any& data) const
        -> std::vector<ValuePathItem> override
    {
        constexpr auto make_path_index_items =
            [](size_t size)
            {
                auto result = std::vector<ValuePathItem>{};
                result.reserve(size);
                for (size_t index=0; index<size; ++index)
                    result.emplace_back(index);
                return result;
            };
        constexpr auto tag = common::Type<T>;
        const auto& v = unpack(data, tag);
        if constexpr (common::StructType<T>)
        {
            constexpr auto fields = field_names_of(tag);
            return std::vector<ValuePathItem>( fields.begin(), fields.end() );
        }
        else if constexpr (requires { std::size(v); })
            return make_path_index_items(std::size(v));
        else if constexpr (common::is_tuple_like_v<T>)
            return make_path_index_items(std::tuple_size_v<T>);
        else
            return {};
    }

    auto get(ValuePathView path, const std::any& data) const
        -> std::pair<const Type*, std::any> override
    {
        return ValueComponents<Type, T>::dispatch(
            path,
            unpack<T>(data),
            [](const auto& v, auto tag) -> std::pair<const Type*, std::any>
            { return {Type::of(tag), v}; });
    }

    auto set(ValuePathView path, std::any& data, const std::any& src) const
        -> void override
    {
        ValueComponents<Type, T>::dispatch(
            path,
            unpack<T>(data),
            [&](auto& v, auto tag)
            { v = unpack(src, tag); });
    }

    auto size(ValuePathView path, const std::any& data) const
        -> size_t override
    {
        return ValueComponents<Type, T>::dispatch(
            path,
            unpack<T>(data),
            [](const auto& v, auto tag) -> size_t
            {
                if constexpr (requires { std::size(v); })
                    return std::size(v);
                else
                    throw std::invalid_argument(
                        "Objects of this type do not have a size");
            });
    }

    auto resize(ValuePathView path, std::any& data, size_t size) const
        -> void override
    {
        ValueComponents<Type, T>::dispatch(
            path,
            unpack<T>(data),
            [&](auto& v, auto tag)
            {
                if constexpr (requires { v.resize(size); })
                    v.resize(size);
                else
                    throw std::invalid_argument(
                        "Objects of this type cannot be resized");
            });
    }

    auto equal(const std::any& lhs, const std::any& rhs) const
        -> bool override
    {
        if constexpr (actually_equality_comparable<T>)
            return unpack<T>(lhs) == unpack<T>(rhs);
        else
        {
            auto k = keys(lhs);
            if (k != keys(rhs))
                return false;
            for (const auto& key : k)
            {
                auto [tl, dl] = get(ValuePath{key}, lhs);
                auto [tr, dr] = get(ValuePath{key}, rhs);
                if (tl != tr)
                    return false;
                if (!tl->value_component_access()->equal(dl, dr))
                    return false;
            }
            return true;
        }
    }

    auto make_data() const
        -> std::any override
    { return T{}; }
};

// ---

template <typename Type, ScalarType T>
struct ValueComponents<Type, T> final
{
    template <common::MaybeConst<T> U, typename F>
    static auto dispatch(ValuePathView path, U& data, F&& f)
    {
        assert(path.empty());
        return std::invoke(std::forward<F>(f), data, common::Type<T>);
    }
};

template <typename Type>
struct ValueComponents<Type, ValuePath>
{
    template <common::MaybeConst<ValuePath> U, typename F>
    static auto dispatch(ValuePathView path, U& data, F&& f)
    {
        assert(path.empty());
        return std::invoke(std::forward<F>(f), data, common::Type<ValuePath>);
    }
};

template <typename Type, typename T>
struct ValueComponents<Type, std::vector<T>>
{
    using V = std::vector<T>;

    template <common::MaybeConst<V> U, typename F>
    static auto dispatch(ValuePathView path, U& data, F&& f)
    {
        if(path.empty())
            return std::invoke(std::forward<F>(f), data, common::Type<V>);

        auto index = path[0].index();
        auto& element = data.at(index);
        return ValueComponents<Type, T>::dispatch(
            path.subspan(1), element, std::forward<F>(f));
    }
};

template <typename Type, typename T, size_t N>
struct ValueComponents<Type, std::array<T, N>>
{
    using V = std::array<T, N>;

    template <common::MaybeConst<V> U, typename F>
    static auto dispatch(ValuePathView path, U& data, F&& f)
    {
        if(path.empty())
            return std::invoke(std::forward<F>(f), data, common::Type<V>);

        auto index = path[0].index();
        auto& element = data.at(index);
        return ValueComponents<Type, T>::dispatch(
            path.subspan(1), element, std::forward<F>(f));
    }
};

template <typename Type, typename... Ts>
struct ValueComponents<Type, std::tuple<Ts...>>
{
    using V = std::tuple<Ts...>;

    template <common::MaybeConst<V> U, typename F>
    static auto dispatch(ValuePathView path, U& data, F&& f)
    {
        if(path.empty())
            return std::invoke(std::forward<F>(f), data, common::Type<V>);

        auto index = path[0].index();

        return visit_index(
            std::integral_constant<size_t, sizeof...(Ts)>{},
            index,
            [&]<size_t I>(std::integral_constant<size_t, I>)
            {
                auto& element = std::get<I>(data);
                using E = std::remove_cvref_t<decltype(element)>;
                return ValueComponents<Type, E>::dispatch(
                    path.subspan(1), element, std::forward<F>(f));
            });
    }
};

template <typename Type, common::StructType T>
struct ValueComponents<Type, T>
{
    template <common::MaybeConst<T> U, typename F>
    static auto dispatch(ValuePathView path, U& data, F&& f)
    {
        if(path.empty())
            return std::invoke(std::forward<F>(f), data, common::Type<T>);

        auto field_name = path[0].name();
        constexpr auto field_names = field_names_of(common::Type<T>);
        auto it = std::find(field_names.begin(), field_names.end(), field_name);
        assert(it != field_names.end());
        auto index = it - field_names.begin();
        auto fields = fields_of(data);
        constexpr auto field_count = field_names.size();

        return visit_index(
            std::integral_constant<size_t, field_count>{},
            index,
            [&]<size_t I>(std::integral_constant<size_t, I>)
            {
                auto& element = std::get<I>(fields);
                using E = std::remove_cvref_t<decltype(element)>;
                return ValueComponents<Type, E>::dispatch(
                    path.subspan(1), element, std::forward<F>(f));
            });
    }
};

template <typename Type, RegisteredCustomType T>
struct ValueComponents<Type, T>
{
    template <common::MaybeConst<T> U, typename F>
    static auto dispatch(ValuePathView path, U& data, F&& f)
    {
        assert(path.empty());
        return std::invoke(std::forward<F>(f), data, common::Type<T>);
    }
};

template <typename Type, StringType T>
struct ValueComponents<Type, T>
{
    template <common::MaybeConst<T> U, typename F>
    static auto dispatch(ValuePathView path, U& data, F&& f)
    {
        assert(path.empty());
        return std::invoke(std::forward<F>(f), data, common::Type<T>);
    }
};

template <typename Type, common::StrongType T>
struct ValueComponents<Type, T>
{
    template <common::MaybeConst<T> U, typename F>
    static auto dispatch(ValuePathView path, U& data, F&& f)
    {
        if(path.empty())
            return std::invoke(std::forward<F>(f), data, common::Type<T>);

        auto name = path[0].name();
        if (name != "v")
            common::throw_(
                "For a strong type, the only possible path key is 'v'. "
                "Got '", name, "' for type ", Type::template of<T>());

        using Weak = typename T::Weak;
        auto& weak = data.v;
        return ValueComponents<Type, Weak>::dispatch(
            path.subspan(1), weak, std::forward<F>(f));
    }
};

template <typename T>
using ValueComponentsAccessFactoryFunc =
    std::unique_ptr<ValueComponentAccess<Type>>(*)();

template <typename Type, typename T>
auto value_components_access_factory(common::Type_Tag<Type> = {},
                                     common::Type_Tag<T> = {})
    -> ValueComponentsAccessFactoryFunc<Type>
{
    constexpr auto result = +[]()
        -> std::unique_ptr<ValueComponentAccess<Type>>
    {
        return std::make_unique< ValueComponentAccessImpl<Type, T> >();
    };
    return result;
}

} // namespace gc::detail
