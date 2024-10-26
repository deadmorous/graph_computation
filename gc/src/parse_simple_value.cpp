#include "gc/parse_simple_value.hpp"

#include "gc/value.hpp"

#include <charconv>

using namespace std::string_view_literals;

namespace gc {

namespace {

template <typename T, typename... Fcargs>
requires (std::is_integral_v<T> || std::is_floating_point_v<T>)
auto parse_as_decimal(common::Type_Tag<T>, std::string_view s, Fcargs... fcargs)
    -> T
{
    auto result = T{};
    auto fcres = std::from_chars(s.begin(), s.end(), result, fcargs...);

    if (fcres.ec == std::errc::invalid_argument)
        common::throw_(
            "Failed to parse scalar of type ", type_of<T>(),
            " from string '", s, "' - not a number");

    else if (fcres.ec == std::errc::result_out_of_range)
        common::throw_(
            "Failed to parse scalar of type ", type_of<T>(),
            " from string '", s, "' - out of range");

    assert (fcres.ec == std::error_code{});
    if(fcres.ptr != s.end())
        common::throw_(
            "Failed to parse scalar of type ", type_of<T>(),
            " from string '", s, "' - extra characters remain");

    return result;
}

struct ScalarParser final
{
    template <typename T>
    requires std::is_integral_v<T>
    auto operator()(common::Type_Tag<T> tag, std::string_view s) const
        -> Value
    {
        if (s.starts_with("0x"sv))
            return parse_as_decimal(tag, s.substr(2), 16);
        else if (s.starts_with("#"sv))
            return parse_as_decimal(tag, s.substr(1), 16);
        else
            return parse_as_decimal(tag, s);
    }

    template <typename T>
    requires std::is_floating_point_v<T>
    auto operator()(common::Type_Tag<T> tag, std::string_view s) const
        -> Value
    { return parse_as_decimal(tag, s); }

    auto operator()(common::Type_Tag<bool>, std::string_view s) const
        -> Value
    {
        if (s == "true")
            return true;
        else if (s == "false")
            return false;
        else
            common::throw_(
                "Failed to parse boolean scalar from string '", s, "'");
    }

    auto operator()(common::Type_Tag<std::byte>, std::string_view) const
        -> Value
    {
        common::throw_("TODO: Parse std::byte");
    }
};

struct SimpleValueParser final
{
    auto operator()(const CustomT& t, std::string_view text) const
        -> Value
    {
        common::throw_(
            "SimpleValueParser: Failed to parse value of type ", t.type(),
            " because custom types are not supported");
    }

    auto operator()(const PathT& t, std::string_view text) const
        -> Value
    { return ValuePath::from_string(text); }

    auto operator()(const ScalarT& t, std::string_view text) const
        -> Value
    { return t.visit(ScalarParser{}, text); }

    auto operator()(const StringT& t, std::string_view text) const
        -> Value
    { return std::string{ text }; }

    auto operator()(const StrongT& t, std::string_view text) const
        -> Value
    {
        auto weak_value = visit(t.weak_type(), SimpleValueParser{}, text);
        auto result =
            Value::make(t.type());
        result.set(ValuePath{} / "v"sv, weak_value);
        return result;
    }

    auto operator()(const StructT& t, std::string_view text) const
        -> Value
    {
        common::throw_(
            "SimpleValueParser: Failed to parse value of type ", t.type(),
            " because struct types are not supported");
    }

    auto operator()(const TupleT& t, std::string_view text) const
        -> Value
    {
        common::throw_(
            "SimpleValueParser: Failed to parse value of type ", t.type(),
            " because tuple types are not supported");
    }

    auto operator()(const VectorT& t, std::string_view text) const
        -> Value
    {
        common::throw_(
            "SimpleValueParser: Failed to parse value of type ", t.type(),
            " because vector types are not supported");
    }
};

} // anonymous namespace


auto parse_simple_value(std::string_view text, const gc::Type* type)
    -> gc::Value
{
    return visit(type, SimpleValueParser{}, text);
}

} // namespace gc
