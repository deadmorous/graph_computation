#pragma once

#include "gc/type_fwd.hpp"

#include "gc/detail/value_component_access.hpp"
#include "gc/value_path.hpp"

#include "common/impl_tag.hpp"
#include "common/strong.hpp"
#include "common/type.hpp"

#include <array>
#include <cassert>
#include <compare>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <span>
#include <string_view>
#include <tuple>
#include <vector>

#define GC_REGISTER_CUSTOM_TYPE(T, id_)                                     \
    namespace gc {                                                          \
    template <> struct CustomTypeToId<T>                                    \
    {                                                                       \
        using type = T;                                                     \
        static constexpr uint8_t id = id_;                                  \
        static constexpr std::string_view name = #T;                        \
    };                                                                      \
                                                                            \
    template <> struct IdToCustomType<id_>                                  \
    {                                                                       \
        using type = T;                                                     \
        static constexpr uint8_t id = id_;                                  \
    };                                                                      \
    }                                                                       \
    static_assert(true)


namespace gc {

enum class AggregateType : uint8_t
{
    Array,
    Custom,
    Path,
    Scalar,
    String,
    Strong,
    Struct,
    Tuple,
    Vector
};



class Type final
{
public:
    static constexpr size_t max_size = 64;

    using Storage = std::array<std::byte, max_size>;

    using ValueComponentAccess =
        detail::ValueComponentAccess<Type>;

    using ValueComponentsAccessFactoryFunc =
        detail::ValueComponentsAccessFactoryFunc<Type>;

    static constexpr auto this_tag = common::Type<Type>;


    // TODO: Hide it, only need in the source

    struct ByteInitializer final
    {
        ByteInitializer(std::byte data)
            : b{ data }
        {}

        ByteInitializer(AggregateType aggregate_type)
            : b{ *reinterpret_cast<const std::byte*>(&aggregate_type) }
        { static_assert(sizeof(aggregate_type) == sizeof(std::byte)); }

        ByteInitializer(ScalarTypeId scalar_type_id)
            : b{ *reinterpret_cast<const std::byte*>(&scalar_type_id) }
        { static_assert(sizeof(scalar_type_id) == sizeof(std::byte)); }

        ByteInitializer(StringTypeId string_type_id)
            : b{ *reinterpret_cast<const std::byte*>(&string_type_id) }
        { static_assert(sizeof(string_type_id) == sizeof(std::byte)); }

        ByteInitializer(uint8_t size)
            : b{ *reinterpret_cast<const std::byte*>(&size) }
        { static_assert(sizeof(uint8_t) == sizeof(std::byte)); }

        operator std::byte() const
        { return b; }

        std::byte b;
    };

    Type(common::Impl_Tag,
         std::initializer_list<ByteInitializer> init,
         std::initializer_list<const Type*> bases,
         const std::string_view* names)
        : Type{ init, bases, names }
    {}



    template <typename T>
    static auto of()
        -> const Type*
    { return of(common::Type<T>); }

    template <ScalarType T>
    static auto of(common::Type_Tag<T> tag)
        -> const Type*
    {
        return intern(
            detail::value_components_access_factory(this_tag, tag),
            { AggregateType::Scalar, scalar_type_id_of(tag) });
    }

    template <StringType T>
    static auto of(common::Type_Tag<T> tag)
        -> const Type*
    {
        return intern(
            detail::value_components_access_factory(this_tag, tag),
            { AggregateType::String, string_type_id_of(tag) });
    }

    template <common::StrongType T>
    static auto of(common::Type_Tag<T> tag)
        -> const Type*
    {
        return intern(
            detail::value_components_access_factory(this_tag, tag),
            { AggregateType::Strong},
            {of<typename T::Weak>()});
    }

    template <typename T>
    static auto of(common::Type_Tag<std::vector<T>> tag)
        -> const Type*
    {
        return intern(
            detail::value_components_access_factory(this_tag, tag),
            {AggregateType::Vector},
            {of<T>()});
    }

    template <typename T, size_t N>
    static auto of(common::Type_Tag<std::array<T, N>> tag)
        -> const Type*
    {
        static_assert(N <= std::numeric_limits<uint8_t>::max());
        return intern(
            detail::value_components_access_factory(this_tag, tag),
            {AggregateType::Array, N},
            {of<T>()});
    }

    template <typename... Ts>
    static auto of(common::Type_Tag<std::tuple<Ts...>> tag)
        -> const Type*
    {
        return intern(
            detail::value_components_access_factory(this_tag, tag),
            {AggregateType::Tuple, sizeof...(Ts)},
            {of<Ts>()...});
    }

    template <common::StructType T>
    static auto of(common::Type_Tag<T> tag)
        -> const Type*
    {
        static constexpr auto field_names = field_names_of(tag);
        constexpr uint8_t field_count = field_names.size();
        constexpr auto tuple_tag = tuple_tag_of(tag);
        return intern(
            detail::value_components_access_factory(this_tag, tag),
            {AggregateType::Struct, field_count},
            {of(tuple_tag)},
            field_names.data());
    }

    template <RegisteredCustomType T>
    static auto of(common::Type_Tag<T> tag)
        -> const Type*
    {
        return intern(
            detail::value_components_access_factory(this_tag, tag),
            {AggregateType::Custom, CustomTypeToId<T>::id},
            {},
            {&CustomTypeToId<T>::name});
    }

    static auto of(common::Type_Tag<ValuePath> tag)
        -> const Type*
    {
        return intern(
            detail::value_components_access_factory(this_tag, tag),
            {AggregateType::Path});
    }


    auto operator<=>(const Type& that) const
        -> std::strong_ordering
    { return storage_ <=> that.storage_; }

    auto storage() const
        -> const Storage&
    { return storage_; }

    auto aggregate_type() const noexcept
        -> AggregateType
    { return static_cast<AggregateType>(storage_[1]); }

    auto value_component_access() const noexcept
        -> const ValueComponentAccess*
    { return value_component_access_.get(); }

    friend auto operator<<(std::ostream& s, const Type* t)
        -> std::ostream&;

private:

    alignas(size_t) Storage storage_;
    mutable std::unique_ptr<ValueComponentAccess> value_component_access_;

    Type(std::initializer_list<ByteInitializer> init,
         std::initializer_list<const Type*> bases,
         const std::string_view* names);

    static auto intern(
        ValueComponentsAccessFactoryFunc value_component_access_factory,
        std::initializer_list<ByteInitializer> init,
        std::initializer_list<const Type*> bases = {},
        const std::string_view* names = nullptr)
            -> const Type*;
};

template <typename T>
auto type_of(common::Type_Tag<T> tag = {})
    -> const Type*
{ return Type::of(tag); }


class ArrayT final
{
public:
    ArrayT(const Type*) noexcept;
    auto type() const noexcept -> const Type*;
    auto element_count() const noexcept
        -> uint8_t;
    auto element_type() const noexcept
        -> const Type*;

private:
    const Type* type_;
};

class CustomT final
{
public:
    CustomT(const Type*) noexcept;
    auto type() const noexcept -> const Type*;
    auto name() const noexcept -> std::string_view;
    auto id() const noexcept -> uint8_t;

private:
    const Type* type_;
};

class PathT final
{
public:
    PathT(const Type*) noexcept;
    auto type() const noexcept -> const Type*;

private:
    const Type* type_;
};

class ScalarT final
{
public:
    ScalarT(const Type*) noexcept;
    auto type() const noexcept -> const Type*;
    auto id() const noexcept -> ScalarTypeId;
    auto name() const noexcept -> std::string_view;

    template <typename F, typename... Args>
    auto visit(F&& f, Args&&... args) const
    {
        switch(id())
        {
        case ScalarTypeId::Bool:
            return std::invoke(std::forward<F>(f), common::Type<bool>,
                               std::forward<Args>(args)...);
        case ScalarTypeId::Byte:
            return std::invoke(std::forward<F>(f), common::Type<std::byte>,
                               std::forward<Args>(args)...);
        case ScalarTypeId::F32:
            return std::invoke(std::forward<F>(f), common::Type<float>,
                               std::forward<Args>(args)...);
        case ScalarTypeId::F64:
            return std::invoke(std::forward<F>(f), common::Type<double>,
                               std::forward<Args>(args)...);
        case ScalarTypeId::I8:
            return std::invoke(std::forward<F>(f), common::Type<int8_t>,
                               std::forward<Args>(args)...);
        case ScalarTypeId::I16:
            return std::invoke(std::forward<F>(f), common::Type<int16_t>,
                               std::forward<Args>(args)...);
        case ScalarTypeId::I32:
            return std::invoke(std::forward<F>(f), common::Type<int32_t>,
                               std::forward<Args>(args)...);
        case ScalarTypeId::I64:
            return std::invoke(std::forward<F>(f), common::Type<int64_t>,
                               std::forward<Args>(args)...);
        case ScalarTypeId::U8:
            return std::invoke(std::forward<F>(f), common::Type<uint8_t>,
                               std::forward<Args>(args)...);
        case ScalarTypeId::U16:
            return std::invoke(std::forward<F>(f), common::Type<uint16_t>,
                               std::forward<Args>(args)...);
        case ScalarTypeId::U32:
            return std::invoke(std::forward<F>(f), common::Type<uint32_t>,
                               std::forward<Args>(args)...);
        case ScalarTypeId::U64:
            return std::invoke(std::forward<F>(f), common::Type<uint64_t>,
                               std::forward<Args>(args)...);
        }
        __builtin_unreachable();
    }

    template <typename F, typename... Args>
    auto visit_numeric(F&& f, Args&&... args) const
    {
        return visit(
            [&]<typename T>(common::Type_Tag<T> tag)
                -> std::invoke_result_t< F, common::Type_Tag<int>, Args... >
            {
                if constexpr (   std::is_integral_v<T>
                              || std::is_floating_point_v<T>)
                    return std::invoke(std::forward<F>(f), tag,
                                       std::forward<Args>(args)...);
                else
                    throw std::invalid_argument("Value is not numeric");
            });
    }

private:
    const Type* type_;
};

class StringT final
{
public:
    StringT(const Type*) noexcept;
    auto type() const noexcept -> const Type*;
    auto id() const noexcept -> StringTypeId;
    auto name() const noexcept -> std::string_view;

    template <typename F, typename... Args>
    auto visit(F&& f, Args... args) const
    {
        switch(id())
        {
        case StringTypeId::String:
            return std::invoke(std::forward<F>(f),
                               common::Type<std::string>,
                               std::forward<Args>(args)...);
        case StringTypeId::StringView:
            return std::invoke(std::forward<F>(f),
                               common::Type<std::string_view>,
                               std::forward<Args>(args)...);
        }
        __builtin_unreachable();
    }

private:
    const Type* type_;
};

class StrongT final
{
public:
    StrongT(const Type*) noexcept;
    auto type() const noexcept -> const Type*;
    auto weak_type() const noexcept -> const Type*;

private:
    const Type* type_;
};

class TupleT;

class StructT final
{
public:
    StructT(const Type*) noexcept;
    auto type() const noexcept -> const Type*;
    auto tuple_type() const noexcept
        -> const Type*;
    auto field_names() const noexcept
        -> std::span<const std::string_view>;
    auto tuple() const noexcept
        -> TupleT;

private:
    const Type* type_;
};

class TupleT final
{
public:
    TupleT(const Type*) noexcept;
    auto type() const noexcept -> const Type*;
    auto element_count() const noexcept
        -> uint8_t;
    auto element_types() const noexcept
        -> std::span<const Type* const>;

private:
    const Type* type_;
};

class VectorT final
{
public:
    VectorT(const Type*) noexcept;
    auto type() const noexcept -> const Type*;
    auto element_type() const noexcept
        -> const Type*;

private:
    const Type* type_;
};

template <typename F, typename... Args>
auto visit(const Type* type, F&& f, Args&&... args)
{
    switch(type->aggregate_type())
    {
        case AggregateType::Array:
            return std::invoke(
                std::forward<F>(f), ArrayT(type), std::forward<Args>(args)...);
        case AggregateType::Custom:
            return std::invoke(
                std::forward<F>(f), CustomT(type), std::forward<Args>(args)...);
        case AggregateType::Path:
            return std::invoke(
                std::forward<F>(f), PathT(type), std::forward<Args>(args)...);
        case AggregateType::Scalar:
            return std::invoke(
                std::forward<F>(f), ScalarT(type), std::forward<Args>(args)...);
        case AggregateType::String:
            return std::invoke(
                std::forward<F>(f), StringT(type), std::forward<Args>(args)...);
        case AggregateType::Strong:
            return std::invoke(
                std::forward<F>(f), StrongT(type), std::forward<Args>(args)...);
        case AggregateType::Struct:
            return std::invoke(
                std::forward<F>(f), StructT(type), std::forward<Args>(args)...);
        case AggregateType::Tuple:
            return std::invoke(
                std::forward<F>(f), TupleT(type), std::forward<Args>(args)...);
        case AggregateType::Vector:
            return std::invoke(
                std::forward<F>(f), VectorT(type), std::forward<Args>(args)...);
    }
    __builtin_unreachable();
}

} // namespace gc
