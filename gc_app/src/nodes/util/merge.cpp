/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024-2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "gc_app/nodes/util/merge.hpp"

#include "gc_types/uint_vec.hpp"

#include "gc/expect_n_node_args.hpp"
#include "gc/computation_context.hpp"
#include "gc/computation_node.hpp"
#include "gc/node_port_names.hpp"
#include "gc/type_registry.hpp"
#include "gc/value.hpp"


using namespace std::literals;
using namespace gc::literals;

namespace gc_app::util {

using namespace gc_types;

class Merge final :
    public gc::ComputationNode
{
public:
    explicit Merge(const gc::Value& input_count_value,
                   const gc::ComputationContext& context) :
        type_registry_{context.type_registry}
    {
        auto input_count = input_count_value.convert_to<size_t>();
        input_names_.reserve(1 + 2*input_count);
        input_names_.push_back("output_type");
        for (auto index : common::index_range<size_t>(input_count))
        {
            input_names_.push_back(common::format("path_", index));
            input_names_.push_back(common::format("value_", index));
        }

        input_name_views_.reserve(input_names_.size());
        for (const auto& input_name : input_names_)
            input_name_views_.push_back(input_name);
    }

    auto input_names() const
        -> gc::InputNames override
    { return input_name_views_; }

    auto output_names() const
        -> gc::OutputNames override
    { return gc::node_output_names<Merge>( "output"sv ); }

    auto default_inputs(gc::InputValues result) const
        -> void override
    {
        assert(!result.empty());
        assert(result.size().v & 1u);
        result[0_gc_i] = "Vector[I32]"s;
        iterate_inputs(
            result,
            [](size_t index, gc::Value& path, gc::Value& value)
            {
                path = gc::ValuePath{} / index;
                value = index;
            });
    }

    auto compute_outputs(
            gc::OutputValues result,
            gc::ConstInputValues inputs,
            const std::stop_token&,
            const gc::NodeProgress&) const
        -> bool override
    {
        assert(inputs.size().v > 0);
        assert(result.size().v & 1u);
        assert(result.size() == 1_gc_oc);
        auto output_type_name = inputs[0_gc_i].convert_to<std::string_view>();
        auto output_type = type_registry_.at(output_type_name);
        auto result_value = gc::Value::make(output_type);
        iterate_inputs(
            inputs,
            [&](size_t /* index */,
                const gc::Value& path,
                const gc::Value& value)
            {
                result_value.set(path.as<gc::ValuePath>(), value);
            });
        result[0_gc_o] = std::move(result_value);
        return true;
    }

private:
    template <typename Inputs, typename F>
    auto iterate_inputs(Inputs inputs, F&& f) const -> void
    {
        assert(inputs.size().v & 1u);
        auto input_count = inputs.size().v / 2;
        for (auto index : common::index_range<int32_t>(input_count))
        {
            auto& path = inputs[gc::InputPort(1u + 2u*index)];
            auto& value = inputs[gc::InputPort(1u + 2u*index + 1u)];
            f(index, path, value);
        }
    }

    gc::TypeRegistry type_registry_;
    std::vector<std::string> input_names_;
    std::vector<std::string_view> input_name_views_;
};

auto make_merge(gc::ConstValueSpan args, const gc::ComputationContext& context)
    -> std::shared_ptr<gc::ComputationNode>
{
    gc::expect_n_node_args("Merge", args, 1);
    return std::make_shared<Merge>(args[0], context);
}

} // namespace gc_app::util
