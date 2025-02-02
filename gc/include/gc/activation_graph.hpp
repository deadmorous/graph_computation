#include "gc/activation_node_fwd.hpp"
#include "gc/algorithm_fwd.hpp"
#include "gc/graph.hpp"

#include "common/grouped.hpp"

#include <memory>


namespace gc {

using ActivationGraph = Graph<std::shared_ptr<ActivationNode>>;

struct ActivationGraphSourceTypes final
{
    std::vector<alg::id::Type> types;
    common::Grouped<EdgeInputEnd> destinations;

    auto operator==(const ActivationGraphSourceTypes&) const noexcept
        -> bool = default;
};

auto operator<<(std::ostream& s, const ActivationGraphSourceTypes& source_types)
    -> std::ostream&;

auto generate_source(const ActivationGraph& g,
                     const ActivationGraphSourceTypes& source_types = {})
    -> void;

} // namespace gc
