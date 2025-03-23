/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#include "gc/generate_dot.hpp"

#include "gc/activation_node.hpp"

#include "common/throw.hpp"


namespace gc {

auto generate_dot(std::ostream& s,
                  const gc::ActivationGraph& g,
                  gc::NodeLabels node_labels)
    -> void
{
    s << "digraph g {\n";

    assert(node_labels.size() == g.nodes.size());
    for (auto inode : g.nodes.index_range())
    {
        auto* node = g.nodes[inode].get();
        auto label = node_labels[inode];
        s << "  N" << inode
          << " [label=\"" << inode << ": " << label
          << "\\n(" << node->meta().type_name << ")\"]\n";
    }

    s << '\n';

    for (const auto& e : g.edges)
    {
        auto from_port_names = g.nodes.at(e.from.node)->output_names();
        if (!from_port_names.index_range().contains(e.from.port))
            common::throw_("Invalid 'from' port in edge ", e,
                           " - must be less than ",
                           int(from_port_names.size().v));
        auto from_port_name = from_port_names[e.from.port];

        auto to_port_names = g.nodes.at(e.to.node)->input_names();
        if (!to_port_names.index_range().contains(e.to.port))
            common::throw_("Invalid 'to' port in edge ", e,
                           " - must be less than ",
                           int(to_port_names.size().v));
        auto to_port_name = to_port_names[e.to.port];

        s << "  N" << e.from.node << " -> N" << e.to.node
          << " [taillabel=\" " << int(e.from.port.v) << ':' << from_port_name
          << " \" headlabel=\" " << int(e.to.port.v) << ':' << to_port_name
          << " \"]\n";
    }

    s << "}\n";
}

} // namespace gc
