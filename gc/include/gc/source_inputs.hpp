#pragma once

#include "gc/edge.hpp"
#include "gc/value.hpp"

#include "common/grouped.hpp"

#include <ostream>


namespace gc {

struct SourceInputs final
{
    ValueVec values;
    common::Grouped<EdgeEnd> destinations;

    auto operator==(const SourceInputs&) const noexcept -> bool = default;
};

auto operator<<(std::ostream& s, const SourceInputs& source_inputs)
    -> std::ostream&;

} // namespace gc
