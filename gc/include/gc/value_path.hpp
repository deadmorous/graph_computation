#pragma once

#include <ostream>
#include <span>
#include <string_view>
#include <variant>
#include <vector>


namespace gc {

struct ValuePathItem final
{
public:
    ValuePathItem();  // Creates zero index

    /* implicit */ ValuePathItem(size_t index);
    /* implicit */ ValuePathItem(std::string_view name);
    /* implicit */ ValuePathItem(std::string name);

    auto is_index() const noexcept
        -> bool;

    auto is_name() const noexcept
        -> bool;

    auto index() const
        -> size_t;

    auto name() const
        -> std::string_view;

    friend auto operator<<(std::ostream& s, const ValuePathItem& item)
        -> std::ostream&;

    auto operator==(const ValuePathItem&) const noexcept -> bool;

    static auto from_string(std::string_view s)
        -> ValuePathItem;

private:
    using Storage =
        std::variant<size_t, std::string_view, std::string>;

    Storage storage_;
};

class ValuePath :
    public std::vector<ValuePathItem>
{
public:
    using Base = std::vector<ValuePathItem>;

    using Base::Base;

    auto operator/=(const ValuePathItem& i2)
        -> ValuePath&;

    auto operator/=(const ValuePath& p2)
        -> ValuePath&;

    friend auto operator<<(std::ostream& s, const ValuePath& path)
        -> std::ostream&;

    static auto from_string(std::string_view s)
        -> ValuePath;
};

auto operator/(ValuePathItem i1, ValuePathItem i2)
    -> ValuePath;

auto operator/(const ValuePath& p1, ValuePathItem i2)
    -> ValuePath;

auto operator/(const ValuePathItem& i1, ValuePath p2)
    -> ValuePath;

auto operator/(const ValuePath& p1, ValuePath p2)
    -> ValuePath;


using ValuePathView = std::span<const ValuePathItem>;

} // namespace gc
