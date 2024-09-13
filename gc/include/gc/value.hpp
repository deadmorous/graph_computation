#pragma once

#include "gc/type.hpp"

#include "common/type.hpp"

#include <any>
#include <cassert>
#include <cstdint>
#include <memory>
#include <span>
#include <string_view>
#include <vector>

namespace gc {

// -----------

template <typename T>
struct TypedField final
{
    std::string_view        name;
    T                       value;
};

template <typename T>
using TypedFieldVec = std::vector< TypedField<T> >;

using Shape = TypedFieldVec<uint32_t>;

// -----------



struct Value final
{
    AggregateType           aggregate_type;
    std::any                value;
};

using ValueSpan = std::span<Value>;
using ConstValueSpan = std::span<const Value>;
using ValueVec = std::vector<Value>;

struct Array final
{
    Shape                   shape;
    std::shared_ptr<void>   data;
};

struct Scalar final
{
    ScalarTypeId type;
    union {
        std::byte           byte;
        float               f32;
        double              f64;
        int8_t              i8;
        int16_t             i16;
        int32_t             i32;
        int64_t             i64;
        uint8_t             u8;
        uint16_t            u16;
        uint32_t            u32;
        uint64_t            u64;
    } value;
};

struct Field final
{
    std::string_view        name;
    Value                   value;
};

struct Struct final
{
    std::vector<Field>      fields;
};

#define GRAPH_COMPUTATION_DECL_GET_SCALAR(Scalar,                       \
                                          cxx_type,                     \
                                          scalar_type_id,               \
                                          field)                        \
inline auto as(Scalar& v, common::Type_Tag<cxx_type> tag)               \
    -> cxx_type&                                                        \
{                                                                       \
    assert(v.type == ScalarTypeId::scalar_type_id);                     \
    return v.value.field;                                               \
}                                                                       \
static_assert(true)

GRAPH_COMPUTATION_DECL_GET_SCALAR(Scalar, std::byte, Byte, byte);
GRAPH_COMPUTATION_DECL_GET_SCALAR(Scalar, float    , F32 , f32 );
GRAPH_COMPUTATION_DECL_GET_SCALAR(Scalar, double   , F64 , f64 );
GRAPH_COMPUTATION_DECL_GET_SCALAR(Scalar, int8_t   , I8  , i8  );
GRAPH_COMPUTATION_DECL_GET_SCALAR(Scalar, int16_t  , I16 , i16 );
GRAPH_COMPUTATION_DECL_GET_SCALAR(Scalar, int32_t  , I32 , i32 );
GRAPH_COMPUTATION_DECL_GET_SCALAR(Scalar, int64_t  , I64 , i64 );
GRAPH_COMPUTATION_DECL_GET_SCALAR(Scalar, uint8_t  , U8  , u8  );
GRAPH_COMPUTATION_DECL_GET_SCALAR(Scalar, uint16_t , U16 , u16 );
GRAPH_COMPUTATION_DECL_GET_SCALAR(Scalar, uint32_t , U32 , u32 );
GRAPH_COMPUTATION_DECL_GET_SCALAR(Scalar, uint64_t , U64 , u64 );

GRAPH_COMPUTATION_DECL_GET_SCALAR(const Scalar, const std::byte, Byte, byte);
GRAPH_COMPUTATION_DECL_GET_SCALAR(const Scalar, const float    , F32 , f32 );
GRAPH_COMPUTATION_DECL_GET_SCALAR(const Scalar, const double   , F64 , f64 );
GRAPH_COMPUTATION_DECL_GET_SCALAR(const Scalar, const int8_t   , I8  , i8   );
GRAPH_COMPUTATION_DECL_GET_SCALAR(const Scalar, const int16_t  , I16 , i16  );
GRAPH_COMPUTATION_DECL_GET_SCALAR(const Scalar, const int32_t  , I32 , i32  );
GRAPH_COMPUTATION_DECL_GET_SCALAR(const Scalar, const int64_t  , I64 , i64  );
GRAPH_COMPUTATION_DECL_GET_SCALAR(const Scalar, const uint8_t  , U8  , u8   );
GRAPH_COMPUTATION_DECL_GET_SCALAR(const Scalar, const uint16_t , U16 , u16  );
GRAPH_COMPUTATION_DECL_GET_SCALAR(const Scalar, const uint32_t , U32 , u32  );
GRAPH_COMPUTATION_DECL_GET_SCALAR(const Scalar, const uint64_t , U64 , u64  );

template <typename T>
inline auto as(Scalar& v)
    -> T&
{ return as(v, common::Type<T>); }

template <typename T>
inline auto as(const Scalar& v)
    -> T&
{ return as(v, common::Type<T>); }

template <ScalarType T>
auto as(Value& v, common::Type_Tag<T> tag = {})
    -> T&
{
    assert(v.aggregate_type == AggregateType::Scalar);
    return as(std::any_cast<Scalar&>(v.value), tag);
}

} // namespace gc
