/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#pragma once

#include "gc/value_fwd.hpp"

#include "gc/type.hpp"
#include "gc/value_path.hpp"

#include "common/throw.hpp"

#include <any>

namespace gc {

class Value final
{
public:
    // Construction

    Value() noexcept;

    template <typename T, std::convertible_to<T> V>
    Value(common::Type_Tag<T> tag, V&& value) :
        type_{ Type::of(tag) },
        data_{ T(std::forward<V>(value)) }
    {
        static_assert(!(
            std::same_as<T, std::string_view> &&
            std::same_as<std::remove_cvref_t<V>, std::string>),
            "Cannot construct string view from string because of potentially "
            "dangling string view");
    }

    Value(common::Type_Tag<std::string> tag, std::string_view value);

    /* implicit */ Value(const Value&);
    /* implicit */ Value(Value&&);

    /* implicit */ Value(Value&);

    template <typename T>
    /* implicit */ Value(T&& value) :
        Value{ common::Type<std::remove_cvref_t<T>>, std::forward<T>(value) }
    {}

    auto operator=(const Value&) -> Value&;
    auto operator=(Value&&) -> Value&;


    // Field access

    auto type() const noexcept
        -> const Type*;

    auto data() noexcept
        -> std::any&;

    auto data() const noexcept
        -> const std::any&;


    // Interface for C++ type-unaware users

    // Value tree navigation and editing

    auto path_item_keys() const
        -> std::vector<ValuePathItem>;

    auto get(ValuePathView path) const
        -> Value;

    auto set(ValuePathView path, const Value& v)
        -> void;

    auto set_default(ValuePathView path)
        -> void;

    auto set_default()
        -> void;

    // Operations for vector-like values

    auto size(ValuePathView path) const
        -> size_t;

    auto size() const
        -> size_t;

    auto resize(ValuePathView path, size_t size)
        -> void;

    auto resize(size_t size)
        -> void;

    // Operations for set-like values

    auto keys() const -> std::vector<Value>;

    auto contains(const Value& key) const -> bool;

    auto insert(ValuePathView path, const Value& key) -> void;

    auto insert(const Value& key) -> void;

    auto remove(ValuePathView path, const Value& key) -> void;

    auto remove(const Value& key) -> void;

    // ---

    auto operator==(const Value& that) const
        -> bool;

    static auto make(const Type* type)
        -> Value;

    // Interface for C++ type-aware users

    template <typename T>
    auto as(common::Type_Tag<T> = {})
        -> T&
    { return std::any_cast<T&>(data_); }

    template <typename T>
    auto as(common::Type_Tag<T> = {}) const
        -> const T&
    { return std::any_cast<const T&>(data_); }

    template <typename T>
    requires (std::is_integral_v<T> || std::is_floating_point_v<T>)
    auto convert_to(common::Type_Tag<T> = {}) const
        -> T
    {
        if(type_->aggregate_type() != gc::AggregateType::Scalar)
            common::throw_<std::invalid_argument>(
                "Value::convert_to: Expected a scalar argument, got ", type_);

        return
            gc::ScalarT(type_).visit_numeric(
                [&](auto tag) -> T
                { return static_cast<T>(as(tag)); });
    }

    template <StringType T>
    auto convert_to(common::Type_Tag<T> = {}) const
        -> T
    {
        if(type_->aggregate_type() != gc::AggregateType::String)
            common::throw_<std::invalid_argument>(
                "Value::convert_to: Expected a string argument, got ", type_);

        return
            gc::StringT(type_).visit(
                [&](auto tag) -> T
                { return T{ as(tag) }; });
    }

    friend auto operator<<(std::ostream& s, const Value& v)
        -> std::ostream&;

private:
    Value(const Type* type, std::any data);

    const Type* type_{};
    std::any data_;
};


using ValueVec = std::vector<Value>;

} // namespace gc
