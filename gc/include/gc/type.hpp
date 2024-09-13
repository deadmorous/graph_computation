#pragma once

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

namespace common {
constexpr inline struct Impl_Tag final {} Impl;
} // namespace common

namespace gc {

enum class ScalarTypeId : uint8_t
{
    Bool,
    Byte,
    F32,
    F64,
    I8,
    I16,
    I32,
    I64,
    U8,
    U16,
    U32,
    U64
};

#define GRAPH_COMPUTATION_DECL_SCALAR_TYPE_ID(T, id)                        \
    constexpr inline auto scalar_type_id_of(common::Type_Tag<T>) noexcept   \
        -> ScalarTypeId                                                     \
    { return ScalarTypeId::id; }                                            \
    static_assert(true)

GRAPH_COMPUTATION_DECL_SCALAR_TYPE_ID(bool     , Bool);
GRAPH_COMPUTATION_DECL_SCALAR_TYPE_ID(std::byte, Byte);
GRAPH_COMPUTATION_DECL_SCALAR_TYPE_ID(float    , F32 );
GRAPH_COMPUTATION_DECL_SCALAR_TYPE_ID(double   , F64 );
GRAPH_COMPUTATION_DECL_SCALAR_TYPE_ID(int8_t   , I8  );
GRAPH_COMPUTATION_DECL_SCALAR_TYPE_ID(int16_t  , I16 );
GRAPH_COMPUTATION_DECL_SCALAR_TYPE_ID(int32_t  , I32 );
GRAPH_COMPUTATION_DECL_SCALAR_TYPE_ID(int64_t  , I64 );
GRAPH_COMPUTATION_DECL_SCALAR_TYPE_ID(uint8_t  , U8  );
GRAPH_COMPUTATION_DECL_SCALAR_TYPE_ID(uint16_t , U16 );
GRAPH_COMPUTATION_DECL_SCALAR_TYPE_ID(uint32_t , U32 );
GRAPH_COMPUTATION_DECL_SCALAR_TYPE_ID(uint64_t , U64 );

template <typename T>
concept ScalarType =
    requires{ scalar_type_id_of(common::Type<T>); };

template <typename T>
concept StructType =
    requires(T t, const T ct)
{
    fields_of(t);
    fields_of(ct);
    tuple_tag_of(common::Type<T>);
    field_names_of(common::Type<T>);
};

enum class AggregateType : uint8_t
{
    Array,
    Custom,
    Scalar,
    Struct,
    Tuple,
    Vector
};



template <typename> struct CustomTypeToId;
template <uint8_t> struct IdToCustomType;


template <typename T>
concept RegisteredCustomType =
    requires
{
    CustomTypeToId<T>::id;
    CustomTypeToId<T>::name;
    requires IdToCustomType<CustomTypeToId<T>::id>::id == CustomTypeToId<T>::id;
};


class Type final
{
public:
    static constexpr size_t max_size = 64;

    using Storage = std::array<std::byte, max_size>;


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
        return intern({
            AggregateType::Scalar,
            scalar_type_id_of(tag) });
    }

    template <typename T>
    static auto of(common::Type_Tag<std::vector<T>>)
        -> const Type*
    { return intern({AggregateType::Vector}, {of<T>()}); }

    template <typename... Ts>
    static auto of(common::Type_Tag<std::tuple<Ts...>>)
        -> const Type*
    { return intern({AggregateType::Tuple, sizeof...(Ts)}, {of<Ts>()...}); }

    template <StructType T>
    static auto of(common::Type_Tag<T> tag)
        -> const Type*
    {
        static constexpr auto field_names = field_names_of(tag);
        constexpr uint8_t field_count = field_names.size();
        constexpr auto tuple_tag = tuple_tag_of(tag);
        return intern({AggregateType::Struct, field_count},
                      {of(tuple_tag)},
                      field_names.data());
    }

    template <RegisteredCustomType T>
    static auto of(common::Type_Tag<T> tag)
        -> const Type*
    {
        return intern({AggregateType::Custom, CustomTypeToId<T>::id},
                      {},
                      {&CustomTypeToId<T>::name});
    }


    auto operator<=>(const Type&) const
        -> std::strong_ordering = default;

    auto storage() const
        -> const Storage&
    { return storage_; }

    auto aggregate_type() const noexcept
        -> AggregateType
    { return static_cast<AggregateType>(storage_[1]); }

    friend auto operator<<(std::ostream& s, const Type* t)
        -> std::ostream&;

private:

    alignas(size_t) Storage storage_;

    Type(std::initializer_list<ByteInitializer> init,
         std::initializer_list<const Type*> bases,
         const std::string_view* names);

    static auto intern(std::initializer_list<ByteInitializer> init,
                       std::initializer_list<const Type*> bases = {},
                       const std::string_view* names = nullptr)
        -> const Type*;
};

template <typename T>
auto type_of(common::Type_Tag<T> tag = {})
    -> Type
{ return Type::of(tag); }


class CustomT final
{
public:
    CustomT(const Type*) noexcept;
    auto name() const noexcept -> std::string_view;
    auto id() const noexcept -> uint8_t;

private:
    const Type* type_;
};

class ScalarT final
{
public:
    ScalarT(const Type*) noexcept;
    auto id() const noexcept -> ScalarTypeId;

private:
    const Type* type_;
};

class TupleT;

class StructT final
{
public:
    StructT(const Type*) noexcept;
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
            assert(false); // TODO
            // Fallthrough
        case AggregateType::Custom:
            return std::invoke(
                std::forward<F>(f), CustomT(type), std::forward<Args>(args)...);
        case AggregateType::Scalar:
            return std::invoke(
                std::forward<F>(f), ScalarT(type), std::forward<Args>(args)...);
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
}

} // namespace gc
