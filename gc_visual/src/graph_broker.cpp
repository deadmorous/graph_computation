#include "gc_visual/graph_broker.hpp"

#include "gc/source_param.hpp"

GraphBroker::GraphBroker(gc::Graph& g,
                         const gc::detail::NamedNodes& named_nodes,
                         QObject* parent) :
    QObject{ parent },
    g_{ g },
    named_nodes_{ named_nodes }
{
    for (uint32_t index=0; const auto& node: g.nodes)
        node_indices_[node.get()] = index++;
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

auto GraphBroker::set_parameter(const gc::ParameterSpec& spec,
                                const gc::Value& value)
    -> void
{
    auto inputs = std::vector<gc::Value>(spec.node->output_count());
    gc::InputParameters::get(spec.node)->get_inputs(inputs);
    inputs.at(spec.index).set(spec.path, value);
    gc::InputParameters::get(spec.node)->set_inputs(inputs);

    // TODO: Compute graph, emit `output_updated`.
}
