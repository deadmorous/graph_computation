#pragma once

#include "gc/type_fwd.hpp"
#include "gc/value_path.hpp"

#include <any>
#include <cassert>
#include <functional>
#include <stdexcept>


namespace gc::detail {

struct AnyValueComponentAccess
{
    virtual ~AnyValueComponentAccess() = default;

    virtual auto get(ValuePathView path,
                     const std::any& data) const
        -> std::any = 0;

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
auto unpack(std::any& data)
    -> T&
{ return std::any_cast<T&>(data); }

template <typename T>
auto unpack(const std::any& data)
    -> const T&
{ return std::any_cast<const T&>(data); }

template <size_t N, typename F, typename... Args>
auto visit_index(std::integral_constant<size_t, N>,
                 size_t index,
                 F&& f,
                 Args&&... args)
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

template <typename T>
struct ValueComponentAccess;

template <typename T>
struct AnyValueComponentAccessImpl final : AnyValueComponentAccess
{
    auto get(ValuePathView path, const std::any& data) const
        -> std::any override
    { return ValueComponentAccess<T>::get(path, unpack<T>(data)); }

    auto set(ValuePathView path, std::any& data, const std::any& src) const
        -> void override
    { ValueComponentAccess<T>::set(path, unpack<T>(data), src); }

    auto size(ValuePathView path, const std::any& data) const
        -> size_t override
    { return ValueComponentAccess<T>::size(path, unpack<T>(data)); }

    auto resize(ValuePathView path, std::any& data, size_t size) const
        -> void override
    { ValueComponentAccess<T>::resize(path, unpack<T>(data), size); }
};

template <ScalarType T>
struct ValueComponentAccess<T> final
{
    static auto get(ValuePathView path, const T& data)
        -> std::any
    {
        assert(path.empty());
        return data;
    }

    static auto set(ValuePathView path, T& data, const std::any& src)
        -> void
    {
        assert(path.empty());
        data = unpack<T>(src);
    }

    static auto size(ValuePathView path, const T& data)
        -> size_t
    { return 0; }

    static auto resize(ValuePathView path, std::any& data, size_t size)
        -> void
    {}
};


template <typename T>
struct ValueComponentAccess<std::vector<T>>
{
    using V = std::vector<T>;

    static auto get(ValuePathView path, const V& data)
        -> std::any
    {
        if(path.empty())
            return data;

        auto index = std::get<size_t>(path[0]);
        const auto& element = data.at(index);
        return ValueComponentAccess<T>::get(path.subspan(1), element);
    }

    static auto set(ValuePathView path, V& data, const std::any& src)
        -> void
    {
        if(path.empty())
        {
            data = unpack<V>(src);
            return;
        }

        auto index = std::get<size_t>(path[0]);
        auto& element = data.at(index);
        return ValueComponentAccess<T>::set(path.subspan(1), element, src);
    }

    static auto size(ValuePathView path, const V& data)
        -> size_t
    { return data.size(); }

    static auto resize(ValuePathView path, V& data, size_t size)
        -> void
    { data.resize(size); }
};

template <typename... Ts>
struct ValueComponentAccess<std::tuple<Ts...>>
{
    using V = std::tuple<Ts...>;

    static auto get(ValuePathView path, const V& data)
        -> std::any
    {
        if(path.empty())
            return data;

        auto index = std::get<size_t>(path[0]);

        return visit_index(
            std::integral_constant<size_t, sizeof...(Ts)>{},
            index,
            [&]<size_t I>(std::integral_constant<size_t, I>)
            {
                const auto& element = std::get<I>(data);
                using T = std::remove_cvref_t<decltype(element)>;
                return ValueComponentAccess<T>::get(path.subspan(1), element);
            });
    }

    static auto set(ValuePathView path, V& data, const std::any& src)
        -> void
    {
        if(path.empty())
        {
            data = unpack<V>(src);
            return;
        }

        auto index = std::get<size_t>(path[0]);

        return visit_index(
            std::integral_constant<size_t, sizeof...(Ts)>{},
            index,
            [&]<size_t I>(std::integral_constant<size_t, I>)
            {
                auto& element = std::get<I>(data);
                using T = std::remove_cvref_t<decltype(element)>;
                return ValueComponentAccess<T>::set(path.subspan(1),
                                                    element,
                                                    src);
            });
    }

    static auto size(ValuePathView path, const V& data)
        -> size_t
    { return sizeof...(Ts); }

    static auto resize(ValuePathView path, V& data, size_t size)
        -> void
    {}
};

template <StructType T>
struct ValueComponentAccess<T>
{
    // TODO
};

} // namespace gc::detail
