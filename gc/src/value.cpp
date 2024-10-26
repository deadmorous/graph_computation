#include "gc/value.hpp"

#include "common/format.hpp"


using namespace std::string_view_literals;

namespace gc {

namespace {

class ScalarFormatter final
{
public:
    template <typename T>
    requires (std::is_integral_v<T> || std::is_floating_point_v<T>)
    auto operator()(common::Type_Tag<T> tag, const Value& v) const
        -> std::string
    {
        if constexpr (sizeof(T) == 1)
            return common::format(v.convert_to<int>());

        return common::format(v.as(tag));
    }

    auto operator()(common::Type_Tag<bool> tag, const Value& v) const
        -> std::string
    {
        auto b = v.as(tag);
        return b ? "true" : "false";
    }

    auto operator()(common::Type_Tag<std::byte>tag, const Value& v) const
        -> std::string
    {
        auto b = static_cast<uint8_t>(v.as(tag));
        constexpr auto hex_chars = "0123456789abcdef";
        auto result = std::string("xx");
        result[0] = hex_chars[b>>4];
        result[1] = hex_chars[b&0xf];
        return result;
    }
};

class ValueFormatter final
{
public:
    explicit ValueFormatter(const Value& value) :
        result_{ visit(value.type(), *this, value) }
    {}

    auto operator()(const gc::CustomT& t, const Value& value) const
        -> std::string
    { return "custom"; }

    auto operator()(const gc::PathT& t, const Value& value) const
        -> std::string
    { return common::format(value.as<ValuePath>()); }

    auto operator()(const gc::ScalarT& t, const Value& value) const
        -> std::string
    { return t.visit(ScalarFormatter{}, value); }

    auto operator()(const gc::StringT& t, const Value& value) const
        -> std::string
    { return value.convert_to<std::string>(); }

    auto operator()(const gc::StrongT& t, const Value& value) const
        -> std::string
    {
        // Format as weakly-typed value - TODO better
        return ValueFormatter{ value.get( ValuePath{ "v"sv } ) };
    }

    auto operator()(const gc::StructT& t, const Value& value) const
        -> std::string
    {
        auto field_names = t.field_names();
        auto tuple = t.tuple();
        auto n = tuple.element_count();
        std::ostringstream s;
        s << '{';
        auto delim = "";
        for (uint8_t i=0; i<n; ++i, delim=",")
        {
            auto field_name = field_names[i];
            auto field = value.get(ValuePath{field_name});
            s << delim << field_name
              << '=' << std::string{ ValueFormatter{ field } };
        }
        s << '}';
        return s.str();
    }

    auto operator()(const gc::TupleT& t, const Value& value) const
        -> std::string
    {
        auto n = t.element_count();
        std::ostringstream s;
        s << '{';
        auto delim = "";
        for (uint8_t i=0; i<n; ++i, delim=",")
        {
            auto field = value.get(ValuePath{i});
            s << delim << std::string{ ValueFormatter{ field } };
        }
        s << '}';
        return s.str();
    }

    auto operator()(const gc::VectorT& t, const Value& value) const
        -> std::string
    {
        auto n = value.size();
        std::ostringstream s;
        s << '[';
        auto delim = "";
        for (size_t i=0; i<n; ++i, delim=",")
        {
            auto element = value.get(ValuePath{i});
            s << delim << std::string{ ValueFormatter{ element } };
        }
        s << ']';
        return s.str();
    }

    operator std::string() && noexcept
    { return result_; }

private:

    std::string result_;
};

} // anonymous namespace


Value::Value() noexcept = default;

Value::Value(common::Type_Tag<std::string> tag, std::string_view value) :
    Value(tag, std::string(value))
{}

Value::Value(const Value&) = default;
Value::Value(Value&&) = default;

Value::Value(Value&) = default;

auto Value::operator=(const Value&) -> Value& = default;
auto Value::operator=(Value&&) -> Value& = default;


auto Value::type() const noexcept
    -> const Type*
{ return type_; }

auto Value::data() noexcept
    -> std::any&
{ return data_; }

auto Value::data() const noexcept
    -> const std::any&
{ return data_; }


auto Value::keys() const
    -> std::vector<ValuePathItem>
{ return type_->value_component_access()->keys(data_); }

auto Value::get(ValuePathView path) const
    -> Value
{
    auto [t, d] = type_->value_component_access()->get(path, data_);
    return { t, std::move(d) };
}

auto Value::set(ValuePathView path, const Value& v)
    -> void
{ type_->value_component_access()->set(path, data_, v.data_); }

auto Value::size(ValuePathView path) const
    -> size_t
{ return type_->value_component_access()->size(path, data_); }

auto Value::size() const
    -> size_t
{ return type_->value_component_access()->size({}, data_); }

auto Value::resize(ValuePathView path, size_t size)
    -> void
{ return type_->value_component_access()->resize(path, data_, size); }

auto Value::resize(size_t size)
    -> void
{ return resize({}, size); }

auto Value::make(const Type* type)
    -> Value
{ return { type, type->value_component_access()->make_data() }; }

Value::Value(const Type* type, std::any data) :
    type_{ type },
    data_{ std::move(data) }
{}

auto operator<<(std::ostream& s, const Value& v)
    -> std::ostream&
{
    s << std::string{ ValueFormatter{ v } };
    return s;
}

} // namespace gc
