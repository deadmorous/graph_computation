#pragma once

#include "gc/computation_node_fwd.hpp"
#include "gc/node_port_tags.hpp"
#include "gc/port_values.hpp"
#include "gc/weak_port.hpp"

#include "common/const_name_span.hpp"

#include <concepts>
#include <cstdint>
#include <string>
#include <vector>


namespace gc {

template <typename NodeImpl,
          std::same_as<std::string_view>... Args>
auto node_input_names(common::Type_Tag<NodeImpl>, Args... names)
    -> InputNames
{
    return InputNames{common::const_name_span(
        common::TypePack<NodeImpl, Input_Tag>,
        names...)};
}

template <typename NodeImpl,
         std::same_as<std::string_view>... Args>
auto node_input_names(Args... names)
    -> InputNames
{ return node_input_names(common::Type<NodeImpl>, names...); }



template <typename NodeImpl,
         std::same_as<std::string_view>... Args>
auto node_output_names(common::Type_Tag<NodeImpl>, Args... names)
    -> OutputNames
{
    return OutputNames{common::const_name_span(
        common::TypePack<NodeImpl, Output_Tag>,
        names...)};
}

template <typename NodeImpl,
         std::same_as<std::string_view>... Args>
auto node_output_names(Args... names)
    -> OutputNames
{ return node_output_names(common::Type<NodeImpl>, names...); }

// ---

class DynamicNames
{
public:
    explicit DynamicNames(std::string_view prefix, WeakPort count = 0);
    auto resize(WeakPort count) -> void;
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
    explicit DynamicInputNames(InputPortCount count = common::Zero) :
        DynamicNames{ "in_", count.v }
    {}

    auto operator()() const
        -> InputNames
    { return InputNames{ DynamicNames::operator()() }; }
};

class DynamicOutputNames final :
    public DynamicNames
{
public:
    explicit DynamicOutputNames(OutputPortCount count = common::Zero) :
        DynamicNames{ "out_", count.v }
    {}

    auto operator()() const
        -> OutputNames
    { return OutputNames{ DynamicNames::operator()() }; }
};

} // namespace gc
