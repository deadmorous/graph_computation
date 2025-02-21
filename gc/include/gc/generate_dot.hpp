#pragma once

#include "gc/activation_graph.hpp"
#include "gc/node_labels.hpp"

#include <ostream>


namespace gc {

auto generate_dot(std::ostream& s,
                  const gc::ActivationGraph& g,
                  gc::NodeLabels node_labels)
    -> void;

} // namespace gc
