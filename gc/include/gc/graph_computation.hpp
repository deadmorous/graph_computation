#pragma once

#include <any>
#include <array>
#include <cstdint>
#include <memory>
#include <span>
#include <string_view>
#include <vector>

namespace gc {

enum class ScalarType : uint8_t
{
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

enum class AggregateType : uint8_t
{
    Array,
    Scalar,
    Struct
};


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
    ScalarType type;
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

// -----------

struct Node
{
    virtual ~Node() = default;

    virtual auto input_count() const -> uint32_t = 0;

    virtual auto output_count() const -> uint32_t = 0;

    virtual auto default_inputs(ValueSpan result) const -> void = 0;

    virtual auto compute_outputs(ValueSpan result,
                                 ConstValueSpan inputs) const -> void = 0;
};

using NodePtr = std::shared_ptr<Node>;

struct EdgeEnd final
{
    const Node*             node;
    uint32_t                port;
};

using Edge = std::array<EdgeEnd, 2>;

struct Graph final
{
    std::vector<NodePtr>    nodes;
    std::vector<Edge>       edges;
};

// -----------

struct ComputationInstructions;
using ComputationInstructionsPtr = std::shared_ptr<ComputationInstructions>;
auto operator<<(std::ostream& s, const ComputationInstructions& instructions)
    -> std::ostream&;

// -----------

auto compile(const Graph& g)
    -> ComputationInstructionsPtr;

struct ComputationResult
{
    using NodeOutputs = std::vector<ConstValueSpan>;

    ValueVec                all_outputs;
    NodeOutputs             node_outputs;
};

auto compute(ComputationResult& result,
             const Graph& g,
             const ComputationInstructions* instructions)
    -> void;

} // namespace gc
