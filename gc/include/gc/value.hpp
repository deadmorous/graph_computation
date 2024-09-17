#pragma once

#include "gc/type.hpp"
#include "gc/value_path.hpp"

#include <any>

namespace gc {

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
    /* implicit */ Value(const T& value) :
        Value{ common::Type<T>, value }
    {}

    auto operator=(const Value&) -> Value& = default;


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

    auto get(ValuePathView path) const
        -> Value
    {
        auto [t, d] = type_->value_component_access()->get(path, data_);
        return { t, std::move(d) };
    }

    auto set(ValuePathView path, const Value& v)
        -> void
    { type_->value_component_access()->set(path, data_, v.data_); }

    auto size(ValuePathView path) const
        -> size_t
    { return type_->value_component_access()->size(path, data_); }

    auto resize(ValuePathView path, size_t size)
        -> void
    { return type_->value_component_access()->resize(path, data_, size); }


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
    Value(const Type* type, std::any data) :
        type_{ type },
        data_{ std::move(data) }
    {}

    const Type* type_{};
    std::any data_;
};



using ValueSpan = std::span<Value>;
using ConstValueSpan = std::span<const Value>;
using ValueVec = std::vector<Value>;

} // namespace gc
