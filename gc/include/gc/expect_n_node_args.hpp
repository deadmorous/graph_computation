#pragma once

#include "gc/value_fwd.hpp"

#include <cstdint>
#include <string_view>

namespace gc {

auto expect_n_node_args(std::string_view class_name,
                        ConstValueSpan args,
                        uint32_t expected_count)
    -> void;

inline auto expect_no_node_args(std::string_view class_name,
                                ConstValueSpan args)
    -> void
{ expect_n_node_args(class_name, args, 0); }

} // namespace gc
