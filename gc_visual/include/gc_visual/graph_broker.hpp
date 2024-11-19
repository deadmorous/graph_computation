#pragma once

#include "gc_visual/computation_thread.hpp"

#include "gc/detail/named_nodes.hpp"
#include "gc/detail/node_indices.hpp"
#include "gc/node_port_tags.hpp"
#include "gc/value_fwd.hpp"
#include "gc/value_path.hpp"

#include <QObject>

class GraphBroker final :
    public QObject
{
    Q_OBJECT
public:
    explicit GraphBroker(ComputationThread& computation_thread,
                         const gc::detail::NamedNodes& named_nodes,
                         const std::vector<std::string>& input_names,
                         QObject* parent = nullptr);

    auto node(const std::string& name) const
        -> gc::Node*;

    auto named_nodes() const
        -> const gc::detail::NamedNodes&;

    auto node_indices() const
        -> const gc::detail::NodeIndices&;

    auto node_index(const gc::Node* node) const
        -> uint32_t;

    auto input_index(const std::string& input_name) const
        -> uint32_t;

    auto get_parameter(const gc::ParameterSpec&) const
        -> gc::Value;

    auto get_port_value(gc::EdgeOutputEnd port) const
        -> const gc::Value&;

    auto get_port_value(gc::EdgeInputEnd port) const
        -> const gc::Value&;

signals:
    auto output_updated(gc::EdgeOutputEnd output)
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
    const std::vector<std::string>& input_names_;
    gc::detail::NodeIndices node_indices_;

    gc::ComputationResult computation_result_;
};
