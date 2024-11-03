#include "gc_visual/graph_broker.hpp"

#include "gc/source_param.hpp"

namespace {

auto group_value(uint32_t node_index,
                 uint32_t port_index,
                 const common::Grouped<gc::Value>& groups)
    -> const gc::Value&
{
    auto values =
        group(groups, node_index);

    assert(port_index < values.size());
    return values[port_index];
}

} // anonymous namespace


GraphBroker::GraphBroker(ComputationThread& computation_thread,
                         const gc::detail::NamedNodes& named_nodes,
                         QObject* parent) :
    QObject{ parent },
    computation_thread_{ computation_thread },
    named_nodes_{ named_nodes }
{
    const auto& computation =
        computation_thread_.computation();
    for (uint32_t index=0; const auto& node: computation.graph.nodes)
        node_indices_[node.get()] = index++;

    connect(&computation_thread_, &ComputationThread::finished,
            this, &GraphBroker::on_computation_finished);
}

auto GraphBroker::node(const std::string& name) const
    -> gc::Node*
{ return named_nodes_.at(name); }

auto GraphBroker::named_nodes() const
    -> const gc::detail::NamedNodes&
{ return named_nodes_; }

auto GraphBroker::node_index(const gc::Node* node) const
    -> uint32_t
{ return node_indices_.at(node); }

auto GraphBroker::get_parameter(const gc::ParameterSpec& spec) const
    -> gc::Value
{
    auto inputs = std::vector<gc::Value>(spec.node->output_count());
    gc::InputParameters::get(spec.node)->get_inputs(inputs);
    return inputs.at(spec.index).get(spec.path);
}

auto GraphBroker::get_port_value(gc::EdgeEnd port, gc::Output_Tag) const
    -> const gc::Value&
{
    Q_ASSERT(!computation_thread_.isRunning());
    auto& computation = computation_thread_.computation();
    return group_value(
        node_index(port.node), port.port, computation.result.outputs);
}

auto GraphBroker::get_port_value(gc::EdgeEnd port, gc::Input_Tag) const
    -> const gc::Value&
{
    Q_ASSERT(!computation_thread_.isRunning());
    auto& computation = computation_thread_.computation();
    return group_value(
        node_index(port.node), port.port, computation.result.inputs);
}


auto GraphBroker::set_parameter(const gc::ParameterSpec& spec,
                                const gc::Value& value)
    -> void
{
    computation_thread_.stop();

    auto inputs = std::vector<gc::Value>(spec.node->output_count());
    gc::InputParameters::get(spec.node)->get_inputs(inputs);
    inputs.at(spec.index).set(spec.path, value);
    gc::InputParameters::get(spec.node)->set_inputs(inputs);

    computation_thread_.start();
}

auto GraphBroker::on_computation_finished()
    -> void
{
    if (!computation_thread_.ok())
        return; // "Graph computation has been terminated";

    auto& computation = computation_thread_.computation();
    const auto& nodes = computation.graph.nodes;
    const auto& outputs = computation.result.outputs;

    for (uint32_t ng=group_count(outputs), ig=0; ig<ng; ++ig)
    {
        const auto* node = nodes.at(ig).get();
        for (uint32_t np=group(outputs,ig).size(), ip=0; ip<np; ++ip)
        {
            auto port = gc::EdgeEnd{ node, ip };
            emit output_updated(port);
        }
    }
}
