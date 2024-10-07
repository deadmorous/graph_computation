#include "gc_app/project.hpp"

#include "gc_app/types.hpp"

#include "gc/node_port_names.hpp"
#include "gc/value.hpp"


using namespace std::string_view_literals;

namespace gc_app {

class Project final :
    public gc::Node
{
public:
    auto input_names() const
        -> common::ConstNameSpan
    { return gc::node_input_names<Project>( "value"sv, "path"sv ); }

    auto output_names() const
        -> common::ConstNameSpan
    { return gc::node_output_names<Project>( "projection"sv ); }

    auto default_inputs(gc::ValueSpan result) const
        -> void
    {
        result[0] = uint_vec_val({10, 20});
        result[1] = uint_val(gc::ValuePath{} / 0u);
    }

    auto compute_outputs(
            gc::ValueSpan result,
            gc::ConstValueSpan inputs) const
        -> void
    {
        assert(inputs.size() == 2);
        assert(result.size() == 1);
        result[0] = inputs[0].get(inputs[1].as<gc::ValuePath>());
    }
};

auto make_project()
    -> std::shared_ptr<gc::Node>
{ return std::make_shared<Project>(); }

} // namespace gc_app
