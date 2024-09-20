#pragma once

#include "gc/type_fwd.hpp"
#include "gc/value_path.hpp"

#include <any>
#include <cassert>
#include <functional>
#include <memory>
#include <stdexcept>


namespace gc::detail {

template <typename Type>
struct ValueComponentAccess
{
    virtual ~ValueComponentAccess() = default;

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
};

// ---

template <typename T, typename U>
concept MaybeConst =
    std::same_as<T, U> || std::same_as<T, const U>;

// ---

template <typename Type, ScalarType T>
struct ValueComponents<Type, T> final
{
    template <MaybeConst<T> U, typename F>
    static auto dispatch(ValuePathView path, U& data, F&& f)
    {
        assert(path.empty());
        return std::invoke(std::forward<F>(f), data, common::Type<T>);
    }
};

template <typename Type>
struct ValueComponents<Type, ValuePath>
{
    template <MaybeConst<ValuePath> U, typename F>
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

    template <MaybeConst<V> U, typename F>
    static auto dispatch(ValuePathView path, U& data, F&& f)
    {
        if(path.empty())
            return std::invoke(std::forward<F>(f), data, common::Type<V>);

        auto index = std::get<size_t>(path[0]);
        auto& element = data.at(index);
        return ValueComponents<Type, T>::dispatch(
            path.subspan(1), element, std::forward<F>(f));
    }
};

template <typename Type, typename... Ts>
struct ValueComponents<Type, std::tuple<Ts...>>
{
    using V = std::tuple<Ts...>;

    template <MaybeConst<V> U, typename F>
    static auto dispatch(ValuePathView path, U& data, F&& f)
    {
        if(path.empty())
            return std::invoke(std::forward<F>(f), data, common::Type<V>);

        auto index = std::get<size_t>(path[0]);

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

template <typename Type, StructType T>
struct ValueComponents<Type, T>
{
    template <MaybeConst<T> U, typename F>
    static auto dispatch(ValuePathView path, U& data, F&& f)
    {
        if(path.empty())
            return std::invoke(std::forward<F>(f), data, common::Type<T>);

        auto field_name = std::get<std::string_view>(path[0]);
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
    template <MaybeConst<T> U, typename F>
    static auto dispatch(ValuePathView path, U& data, F&& f)
    {
        assert(path.empty());
        return std::invoke(std::forward<F>(f), data, common::Type<T>);
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
