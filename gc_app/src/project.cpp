#include "gc_app/project.hpp"

#include "gc_app/types.hpp"

#include "gc/expect_n_node_args.hpp"
#include "gc/node.hpp"
#include "gc/node_port_names.hpp"
#include "gc/value.hpp"


using namespace std::string_view_literals;

namespace gc_app {

class Project final :
    public gc::Node
{
public:
    auto input_names() const
        -> common::ConstNameSpan override
    { return gc::node_input_names<Project>( "value"sv, "path"sv ); }

    auto output_names() const
        -> common::ConstNameSpan override
    { return gc::node_output_names<Project>( "projection"sv ); }

    auto default_inputs(gc::ValueSpan result) const
        -> void override
    {
        result[0] = uint_vec_val({10, 20});
        result[1] = uint_val(gc::ValuePath{} / 0u);
    }

    auto compute_outputs(
            gc::ValueSpan result,
            gc::ConstValueSpan inputs,
            const std::stop_token& stoken,
            const gc::NodeProgress& progress) const
        -> bool override
    {
        assert(inputs.size() == 2);
        assert(result.size() == 1);
        result[0] = inputs[0].get(inputs[1].as<gc::ValuePath>());
        return true;
    }
};

auto make_project(gc::ConstValueSpan args)
    -> std::shared_ptr<gc::Node>
{
    gc::expect_no_node_args("Project", args);
    return std::make_shared<Project>();
}

} // namespace gc_app
