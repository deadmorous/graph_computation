/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024-2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "gc_app/nodes/util/project.hpp"

#include "gc_types/uint_vec.hpp"

#include "gc/expect_n_node_args.hpp"
#include "gc/computation_node.hpp"
#include "gc/node_port_names.hpp"
#include "gc/value.hpp"


using namespace std::string_view_literals;
using namespace gc::literals;

namespace gc_app::util {

using namespace gc_types;

class Project final :
    public gc::ComputationNode
{
public:
    auto input_names() const
        -> gc::InputNames override
    { return gc::node_input_names<Project>( "value"sv, "path"sv ); }

    auto output_names() const
        -> gc::OutputNames override
    { return gc::node_output_names<Project>( "projection"sv ); }

    auto default_inputs(gc::InputValues result) const
        -> void override
    {
        assert(result.size() == 2_gc_ic);
        result[0_gc_i] = uint_vec_val({10, 20});
        result[1_gc_i] = gc::ValuePath{} / 0u;
    }

    auto compute_outputs(
            gc::OutputValues result,
            gc::ConstInputValues inputs,
            const std::stop_token&,
            const gc::NodeProgress&) const
        -> bool override
    {
        assert(inputs.size() == 2_gc_ic);
        assert(result.size() == 1_gc_oc);
        result.front() = inputs[0_gc_i].get(inputs[1_gc_i].as<gc::ValuePath>());
        return true;
    }
};

auto make_project(gc::ConstValueSpan args, const gc::ComputationContext&)
    -> std::shared_ptr<gc::ComputationNode>
{
    gc::expect_no_node_args("Project", args);
    return std::make_shared<Project>();
}

} // namespace gc_app::util
