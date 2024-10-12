#include "gc/expect_n_node_args.hpp"

#include "gc/value.hpp"

#include "common/throw.hpp"

namespace gc {

auto expect_n_node_args(std::string_view class_name,
                        ConstValueSpan args,
                        uint32_t expected_count)
    -> void
{
    if (args.size() == expected_count)
        return;

    common::throw_<std::invalid_argument>(
        class_name, ": Expected ", expected_count,
        " construction arguments, got ", args.size());
}

} // namespace gc
