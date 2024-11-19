#include "gc_visual/graph_broker.hpp"

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
    -> gc::Node*
{ return named_nodes_.at(name); }

auto GraphBroker::named_nodes() const
    -> const gc::detail::NamedNodes&
{ return named_nodes_; }

auto GraphBroker::node_indices() const
    -> const gc::detail::NodeIndices&
{ return node_indices_; }

auto GraphBroker::node_index(const gc::Node* node) const
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
{ return group_value(port.node.v, port.port.v, computation_result_.outputs); }

auto GraphBroker::get_port_value(gc::EdgeInputEnd port) const
    -> const gc::Value&
{ return group_value(port.node.v, port.port.v, computation_result_.inputs); }

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

    for (uint32_t ng=group_count(outputs), ig=0; ig<ng; ++ig)
    {
        for (uint8_t np=group(outputs,ig).size(), ip=0; ip<np; ++ip)
        {
            auto port = gc::EdgeOutputEnd{ gc::NodeIndex{ig},
                                           gc::OutputPort{ip} };
            emit output_updated(port);
        }
    }
}
