#include "gc/detail/parse_node_port.hpp"

#include "gc/computation_node.hpp"
#include "gc/detail/named_computation_nodes.hpp"
#include "gc/detail/computation_node_indices.hpp"

#include "common/throw.hpp"

#include <algorithm>
#include <charconv>


namespace gc::detail {

namespace {

template <typename Node, typename PortNamesFunc, PortTagType Tag>
auto parse_node_port_impl(Tag,
                          std::string_view ee_str,
                          const NamedNodes<Node>& node_map,
                          const NodeIndices<Node>& node_indices,
                          PortNamesFunc port_names)
    -> EdgeEnd<Tag>
{
    // Parse node name and optional port name/index
    std::string node_name;
    std::string_view port_name;
    if (auto pos = ee_str.find_last_of('.'); pos != std::string::npos)
    {
        node_name = ee_str.substr(0, pos);
        port_name = { ee_str.data() + pos + 1, ee_str.size() - pos - 1 };
    }
    else
        node_name = ee_str;

    // Find graph node referenced by edge end
    auto node_it = node_map.find(node_name);
    if (node_it == node_map.end())
        common::throw_("Node '", node_name, "' is not found");
    const auto* node = node_it->second;

    // Get node port names
    auto pnames = port_names(node);
    if (pnames.empty())
        common::throw_(
            "Edge end '", ee_str, "' is invalid because node has no ports");

    // Resolve port index
    auto port = WeakPort{0};
    if (port_name.empty())
    {
        // Default index 0 can only be used if there is exactly one port
        if (pnames.size() != 1)
            common::throw_(
                "Port name must be specified for node ", node_name,
                ": please specify one of ", common::format_seq(pnames));
    }
    else if (
        // Try parsing as number, treat `port_name` as a name if that fails
        auto fcres =
            std::from_chars(port_name.begin(), port_name.end(), port);
        fcres.ec != std::error_code{} || fcres.ptr != port_name.end())
    {
        // Parsing as number failed, find port index
        auto it = std::find(pnames.begin(), pnames.end(), port_name);
        if (it == pnames.end())
            // No such port
            common::throw_(
                "Node ", node_name, " does not have port ", port_name,
                ": please specify one of ", common::format_seq(pnames));
        port = it - pnames.begin();
    }
    else if (port >= pnames.size())
        // Check port index bounds
        common::throw_(
            "Number of ports in node ", node_name, " is ", pnames.size(),
            ", port index ", port, " is out of range");
    return {node_indices.at(node), Port<Tag>{port}};
}

} // anonymous namespace


template <typename Node>
auto parse_node_port(std::string_view ee_str,
                     const NamedNodes<Node>& node_map,
                     const NodeIndices<Node>& node_indices,
                     Input_Tag)
    -> EdgeInputEnd
{
    auto input_ports = [](const ComputationNode* node)
    { return node->input_names(); };

    return parse_node_port_impl(
        Input, ee_str, node_map, node_indices, input_ports);
}

template <typename Node>
auto parse_node_port(std::string_view ee_str,
                     const NamedNodes<Node>& node_map,
                     const NodeIndices<Node>& node_indices,
                     Output_Tag)
    -> EdgeOutputEnd
{
    auto output_ports = [](const ComputationNode* node)
    { return node->output_names(); };

    return parse_node_port_impl(
        Output, ee_str, node_map, node_indices, output_ports);
}


template
auto parse_node_port<ComputationNode>(std::string_view ee_str,
                     const NamedComputationNodes& node_map,
                     const ComputationNodeIndices& node_indices,
                     Input_Tag)
    -> EdgeInputEnd;

template
auto parse_node_port<ComputationNode>(std::string_view ee_str,
                     const NamedComputationNodes& node_map,
                     const ComputationNodeIndices& node_indices,
                     Output_Tag)
    -> EdgeOutputEnd;

} // namespace gc::detail
