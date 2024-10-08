#pragma once

#include "gc/node_fwd.hpp"

#include "common/const_name_span.hpp"

#include <concepts>
#include <cstdint>
#include <string>
#include <vector>


namespace gc {

constexpr inline struct Input_Tag final {} Input;
constexpr inline struct Output_Tag final {} Output;

template <std::derived_from<Node> NodeImpl,
          std::same_as<std::string_view>... Args>
auto node_input_names(common::Type_Tag<NodeImpl>, Args... names)
    -> common::ConstNameSpan
{
    return common::const_name_span(
        common::TypePack<NodeImpl, Input_Tag>,
        names...);
}

template <std::derived_from<Node> NodeImpl,
         std::same_as<std::string_view>... Args>
auto node_input_names(Args... names)
    -> common::ConstNameSpan
{ return node_input_names(common::Type<NodeImpl>, names...); }



template <std::derived_from<Node> NodeImpl,
         std::same_as<std::string_view>... Args>
auto node_output_names(common::Type_Tag<NodeImpl>, Args... names)
    -> common::ConstNameSpan
{
    return common::const_name_span(
        common::TypePack<NodeImpl, Output_Tag>,
        names...);
}

template <std::derived_from<Node> NodeImpl,
         std::same_as<std::string_view>... Args>
auto node_output_names(Args... names)
    -> common::ConstNameSpan
{ return node_output_names(common::Type<NodeImpl>, names...); }

// ---

class DynamicNames
{
public:
    explicit DynamicNames(std::string_view prefix);
    auto resize(uint32_t count) -> void;
    auto operator()() const
        -> common::ConstNameSpan;

private:
    std::string_view prefix_;
    std::vector<std::string> names_;
    std::vector<std::string_view> name_views_;
};

class DynamicInputNames final :
    public DynamicNames
{
public:
    explicit DynamicInputNames() : DynamicNames{ "in_" }
    {}
};

class DynamicOutputNames final :
    public DynamicNames
{
public:
    explicit DynamicOutputNames() : DynamicNames{ "out_" }
    {}
};

} // namespace gc
