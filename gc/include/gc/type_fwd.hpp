/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024-2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "common/enum_type.hpp"
#include "common/type.hpp"
#include "common/struct_type.hpp"

#include <magic_enum/magic_enum.hpp>

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <string_view>


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

#undef GRAPH_COMPUTATION_DECL_SCALAR_TYPE_ID

template <typename T>
concept ScalarType =
    requires{ scalar_type_id_of(common::Type<T>); };


enum class StringTypeId : uint8_t
{
    String,
    StringView
};

#define GRAPH_COMPUTATION_DECL_STRING_TYPE_ID(T, id)                        \
constexpr inline auto string_type_id_of(common::Type_Tag<T>) noexcept       \
    -> StringTypeId                                                         \
{ return StringTypeId::id; }                                                \
    static_assert(true)

GRAPH_COMPUTATION_DECL_STRING_TYPE_ID(std::string     , String);
GRAPH_COMPUTATION_DECL_STRING_TYPE_ID(std::string_view, StringView);

#undef GRAPH_COMPUTATION_DECL_STRING_TYPE_ID

template <typename T>
concept StringType =
    std::same_as<T, std::string> || std::same_as<T, std::string_view>;


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


template <common::EnumType E>
struct TypedEnumData
{
    static constexpr auto count = magic_enum::enum_count<E>();

    static constexpr auto names = magic_enum::enum_names<E>();

    static constexpr auto values = []<size_t... I>(std::index_sequence<I...>)
    {
        namespace me = magic_enum;
        return std::array<int64_t, sizeof...(I)>{
            me::enum_integer(me::enum_value<E>(I))... };
    }(std::make_index_sequence<count>());
};

struct UntypedEnumData final
{
    template <common::EnumType E>
    constexpr UntypedEnumData(common::Type_Tag<E> = {}):
        names{TypedEnumData<E>::names},
        values{TypedEnumData<E>::values}
    {}

    std::span<const std::string_view> names;
    std::span<const int64_t> values;
};

template <common::EnumType> struct EnumTypeToId;
template <uint8_t> struct IdToEnumType;

template <typename T>
concept RegisteredEnumType =
    requires
{
    requires common::EnumType<T>;
    EnumTypeToId<T>::id;
    EnumTypeToId<T>::name;
    requires IdToEnumType<EnumTypeToId<T>::id>::id == EnumTypeToId<T>::id;
    IdToEnumType<EnumTypeToId<T>::id>::enum_data;
};

class Type;

} // namespace gc
