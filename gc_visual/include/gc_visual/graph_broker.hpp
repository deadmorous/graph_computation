/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024-2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "gc_visual/graph_broker_fwd.hpp"

#include "gc_visual/computation_thread.hpp"
#include "gc_visual/parse_graph_binding.hpp"

#include "gc/detail/named_computation_nodes.hpp"
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
                         const gc::detail::NamedComputationNodes& named_nodes,
                         const std::vector<std::string>& input_names,
                         QObject* parent = nullptr);

    auto node(const std::string& name) const
        -> gc::ComputationNode*;

    auto named_nodes() const
        -> const gc::detail::NamedComputationNodes&;

    auto node_indices() const
        -> const gc::detail::ComputationNodeIndices&;

    auto binding_resolver() -> const gc_visual::BindingResolver&;

    auto node_index(const gc::ComputationNode* node) const
        -> gc::NodeIndex;

    auto input_index(const std::string& input_name) const
        -> uint32_t;

    auto get_parameter(const gc::ParameterSpec&) const
        -> gc::Value;

    auto get_port_value(gc::EdgeOutputEnd port) const
        -> const gc::Value&;

    auto get_port_value(gc::EdgeInputEnd port) const
        -> const gc::Value&;

    auto evolution() const
        -> std::optional<gc_visual::GraphEvolution>;

signals:
    auto output_updated(gc::EdgeOutputEnd output)
        -> void;

public slots:

    auto advance_evolution(size_t skip = 0)
        -> void;

    auto reset_evolution()
        -> void;

    auto set_parameter(const gc::ParameterSpec&, const gc::Value&)
        -> void;

private slots:
    auto on_computation_finished()
        -> void;

private:
    ComputationThread& computation_thread_;
    const gc::detail::NamedComputationNodes& named_nodes_;
    const std::vector<std::string>& input_names_;
    gc_visual::BindingResolver binding_resolver_;

    gc::ComputationResult computation_result_;
};
