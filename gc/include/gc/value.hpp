#pragma once

#include "gc/type.hpp"
#include "gc/value_path.hpp"

#include <any>

namespace gc {

struct ActionOnValue;

class Value final
{
public:
    // Construction

    Value() noexcept = default;

    template <typename T, std::convertible_to<T> V>
    Value(common::Type_Tag<T> tag, const V& value) :
        type_{ Type::of(tag) },
        data_{ T(value) }
    {}

    template <typename T>
    Value(const T& value) :
        Value{ common::Type<T>, value }
    {}


    // Field access

    auto type() const noexcept
        -> const Type*
    { return type_; }

    auto data() noexcept
        -> std::any&
    { return data_; }

    auto data() const noexcept
        -> const std::any&
    { return data_; }


    // Interface for C++ type-unaware users

    // TODO
    // auto act(const Path& path, Action action) -> void;
    auto act(ValuePathView path, ActionOnValue&) -> void {} // TODO


    // Interface for C++ type-aware users

    template <typename T>
    auto as(common::Type_Tag<T> = {})
        -> T&
    { return std::any_cast<T&>(data_); }

    template <typename T>
    auto as(common::Type_Tag<T> = {}) const
        -> const T&
    { return std::any_cast<const T&>(data_); }

private:
    const Type* type_{};
    std::any data_;
};



using ValueSpan = std::span<Value>;
using ConstValueSpan = std::span<const Value>;
using ValueVec = std::vector<Value>;



struct ActionOnValue
{
    enum {Get, Set, Size, Resize} action;
    Value value;
    size_t size{};
};

} // namespace gc
