#pragma once

#include <span>
#include <string_view>
#include <variant>
#include <vector>


namespace gc {

using ValuePathItem =
    std::variant<size_t, std::string_view>;

class ValuePath :
    public std::vector<ValuePathItem>
{
    using Base = std::vector<ValuePathItem>;

    using Base::Base;

    inline auto operator/=(const ValuePathItem& i2)
        -> ValuePath&;

    inline auto operator/=(const ValuePath& p2)
        -> ValuePath&;
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
