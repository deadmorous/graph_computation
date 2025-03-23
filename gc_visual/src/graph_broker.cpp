/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024 MPK Software, St.-Petersburg, Russia
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
    computation_result_{ computation_thread.computation().result }
{
    const auto& computation =
        computation_thread_.computation();
    for (uint32_t index=0; const auto& node: computation.graph.nodes)
        node_indices_[node.get()] = gc::NodeIndex{index++};

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
{ return node_indices_; }

auto GraphBroker::node_index(const gc::ComputationNode* node) const
    -> gc::NodeIndex
{ return node_indices_.at(node); }

auto GraphBroker::input_index(const std::string& input_name) const
    -> uint32_t
{
    auto it = std::find(input_names_.begin(), input_names_.end(), input_name);
    if (it == input_names_.end())
        common::throw_<std::invalid_argument>(
            "Input with name '", input_name, " is not found");
    return it - input_names_.begin();
}

auto GraphBroker::get_parameter(const gc::ParameterSpec& spec) const
    -> gc::Value
{ return computation_thread_.get_parameter(spec); }

auto GraphBroker::get_port_value(gc::EdgeOutputEnd port) const
    -> const gc::Value&
{ return group_value(port.node, port.port, computation_result_.outputs); }

auto GraphBroker::get_port_value(gc::EdgeInputEnd port) const
    -> const gc::Value&
{ return group_value(port.node, port.port, computation_result_.inputs); }

auto GraphBroker::set_parameter(const gc::ParameterSpec& spec,
                                const gc::Value& value)
    -> void
{
    computation_thread_.set_parameter(spec, value);
    computation_thread_.start();
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
