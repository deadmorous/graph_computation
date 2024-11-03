#pragma once

#include "gc_visual/computation_thread.hpp"

#include "gc/detail/named_nodes.hpp"
#include "gc/node_port_tags.hpp"
#include "gc/param_spec.hpp"
#include "gc/value_fwd.hpp"
#include "gc/value_path.hpp"

#include <QObject>

class GraphBroker final :
    public QObject
{
    Q_OBJECT
public:
    explicit GraphBroker(ComputationThread& computation_thread,
                         const gc::detail::NamedNodes& named_nodes_,
                         QObject* parent = nullptr);

    auto node(const std::string& name) const
        -> gc::Node*;

    auto named_nodes() const
        -> const gc::detail::NamedNodes&;

    auto node_index(const gc::Node* node) const
        -> uint32_t;

    auto get_parameter(const gc::ParameterSpec&) const
        -> gc::Value;

    auto get_port_value(gc::EdgeEnd port, gc::Output_Tag) const
        -> const gc::Value&;

    auto get_port_value(gc::EdgeEnd port, gc::Input_Tag) const
        -> const gc::Value&;

signals:
    auto output_updated(gc::EdgeEnd output)
        -> void;

public slots:
    auto set_parameter(const gc::ParameterSpec&, const gc::Value&)
        -> void;

private slots:
    auto on_computation_finished()
        -> void;

private:
    ComputationThread& computation_thread_;
    const gc::detail::NamedNodes& named_nodes_;
    std::unordered_map<const gc::Node*, uint32_t> node_indices_;
};
