/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "gc/expect_n_node_args.hpp"

#include "gc/value.hpp"

#include "mpk/mix/util/throw.hpp"

namespace gc {

auto expect_n_node_args(std::string_view class_name,
                        ConstValueSpan args,
                        uint32_t expected_count)
    -> void
{
    if (args.size() == expected_count)
        return;

    mpk::mix::throw_<std::invalid_argument>(
        "{}: Expected {} construction arguments, got {}",
        class_name, expected_count, args.size());
}

} // namespace gc
