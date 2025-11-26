/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024-2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "gc_visual/graph_broker.hpp"

namespace {

template<common::StrongGroupedType SG, typename Port>
auto group_value(gc::NodeIndex node_index,
                 Port port_index,
                 const SG& groups)
    -> const gc::Value&
{
    auto values =
        group(groups, node_index);

    assert(port_index < values.size());
    return values[port_index];
}

} // anonymous namespace


GraphBroker::GraphBroker(ComputationThread& computation_thread,
                         const gc::detail::NamedComputationNodes& named_nodes,
                         const std::vector<std::string>& input_names,
                         QObject* parent) :
    QObject{ parent },
    computation_thread_{ computation_thread },
    named_nodes_{ named_nodes },
    input_names_{ input_names },
    binding_resolver_{
        computation_thread.computation().graph,
        named_nodes_,
        input_names_ },
    computation_result_{ computation_thread.computation().result }
{
    connect(&computation_thread_, &ComputationThread::finished,
            this, &GraphBroker::on_computation_finished);
}

auto GraphBroker::node(const std::string& name) const
    -> gc::ComputationNode*
{ return named_nodes_.at(name); }

auto GraphBroker::named_nodes() const
    -> const gc::detail::NamedComputationNodes&
{ return named_nodes_; }

auto GraphBroker::node_indices() const
    -> const gc::detail::ComputationNodeIndices&
{ return binding_resolver_.node_indices(); }

auto GraphBroker::binding_resolver() -> const gc_visual::BindingResolver&
{
    return binding_resolver_;
}

auto GraphBroker::node_index(const gc::ComputationNode* node) const
    -> gc::NodeIndex
{ return binding_resolver_.node_index(node); }

auto GraphBroker::io_spec(const std::string& io_name) const
    -> gc::IoSpec
{ return binding_resolver_.io_spec(io_name); }

auto GraphBroker::get_parameter(const gc::ParameterSpec& spec) const
    -> gc::Value
{ return computation_thread_.get_parameter(spec); }

auto GraphBroker::get_port_value(gc::EdgeOutputEnd port) const
    -> const gc::Value&
{ return group_value(port.node, port.port, computation_result_.outputs); }

auto GraphBroker::get_port_value(gc::EdgeInputEnd port) const
    -> const gc::Value&
{ return group_value(port.node, port.port, computation_result_.inputs); }

auto GraphBroker::evolution() const
    -> std::optional<gc_visual::GraphEvolution>
{ return computation_thread_.evolution(); }

auto GraphBroker::advance_evolution(size_t skip)
    -> void
{ computation_thread_.advance_evolution(skip); }

auto GraphBroker::reset_computation()
    -> void
{ computation_thread_.reset_computation(); }

auto GraphBroker::set_parameter(const gc::ParameterSpec& spec,
                                const gc::Value& value)
    -> void
{
    computation_thread_.set_parameter(spec, value);
    computation_thread_.start_computation();
}

auto GraphBroker::invalidate_input(const gc::ParameterSpec& spec)
    -> void
{
    computation_thread_.invalidate_input(spec);
    computation_thread_.start_computation();
}

auto GraphBroker::on_computation_finished()
    -> void
{
    if (!computation_thread_.ok())
        // "Graph computation has been terminated";
        return;

    auto& computation = computation_thread_.computation();
    computation_result_ = computation.result;
    const auto& nodes = computation.graph.nodes;
    const auto& outputs = computation.result.outputs;

    for (auto ig : group_indices(outputs))
    {
        for (auto ip : group(outputs,ig).index_range())
        {
            auto port = gc::EdgeOutputEnd{ ig, ip };
            emit output_updated(port);
        }
    }
}
