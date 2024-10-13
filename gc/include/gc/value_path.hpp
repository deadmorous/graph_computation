#pragma once

#include "common/format.hpp"

#include <ostream>
#include <span>
#include <string_view>
#include <variant>
#include <vector>


namespace gc {

struct ValuePathItem final
{
public:
    ValuePathItem() = default;  // Creates zero index

    /* implicit */ ValuePathItem(size_t index) :
        storage_{ index }
    {}

    /* implicit */ ValuePathItem(std::string_view name) :
        storage_{ name }
    {}

    /* implicit */ ValuePathItem(std::string name) :
        storage_{ name }
    {}

    auto is_index() const noexcept
    { return holds_alternative<size_t>(storage_); }

    auto is_name() const noexcept
    { return !is_index(); }

    auto index() const
        -> size_t
    {
        if (!is_index())
            throw std::invalid_argument(
                "Retrieving index from value path item which is a name");
        return std::get<size_t>(storage_);
    }

    auto name() const
        -> std::string_view
    {
        if (!is_name())
            throw std::invalid_argument(
                "Retrieving name from value path item which is an index");
        if (holds_alternative<std::string_view>(storage_))
            return std::get<std::string_view>(storage_);
        return std::get<std::string>(storage_);
    }

    friend auto operator<<(std::ostream& s, const ValuePathItem& item)
        -> std::ostream&
    {
        std::visit([&](const auto& typed){ s << typed; }, item.storage_);
        return s;
    }

    auto operator==(const ValuePathItem&) const noexcept -> bool = default;

private:
    using Storage =
        std::variant<size_t, std::string_view, std::string>;

    Storage storage_;
};

class ValuePath :
    public std::vector<ValuePathItem>
{
    using Base = std::vector<ValuePathItem>;

    using Base::Base;

    inline auto operator/=(const ValuePathItem& i2)
        -> ValuePath&;

    inline auto operator/=(const ValuePath& p2)
        -> ValuePath&;

    friend auto operator<<(std::ostream& s, const ValuePath& path)
        -> std::ostream&
    {
        s << '/';
        common::format_seq(path, "/");
        return s;
    }
};

inline auto operator/(ValuePathItem i1, ValuePathItem i2)
    -> ValuePath
{ return ValuePath{ i1, i2 }; }

inline auto operator/(const ValuePath& p1, ValuePathItem i2)
    -> ValuePath
{
    auto result = p1;
    result.push_back( i2 );
    return result;
}

inline auto operator/(const ValuePathItem& i1, ValuePath p2)
    -> ValuePath
{
    auto result = ValuePath{i1};
    result.insert(result.end(), p2.begin(), p2.end());
    return result;
}

inline auto operator/(const ValuePath& p1, ValuePath p2)
    -> ValuePath
{
    auto result = p1;
    result.insert(result.end(), p2.begin(), p2.end());
    return result;
}

auto ValuePath::operator/=(const ValuePathItem& i2)
    -> ValuePath&
{ return *this = *this / i2; }

auto ValuePath::operator/=(const ValuePath& p2)
    -> ValuePath&
{ return *this = *this / p2; }


using ValuePathView = std::span<const ValuePathItem>;

} // namespace gc
