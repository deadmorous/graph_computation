/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "gc/edge.hpp"
#include "gc/value.hpp"

#include "common/grouped.hpp"

#include <ostream>


namespace gc {

struct SourceInputs final
{
    ValueVec values;
    common::Grouped<EdgeInputEnd> destinations;

    auto operator==(const SourceInputs&) const noexcept -> bool = default;
};

auto operator<<(std::ostream& s, const SourceInputs& source_inputs)
    -> std::ostream&;

} // namespace gc
