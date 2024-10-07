#include "gc/node_port_names.hpp"

#include "common/format.hpp"


namespace gc {

DynamicNames::DynamicNames(std::string_view prefix)
    : prefix_{ prefix }
{}

auto DynamicNames::resize(uint32_t count)
    -> void
{
    if (names_.size() == count)
        return;

    if (names_.size() > count)
        names_.resize(count);

    else
        for (uint32_t i=names_.size(); i<count; ++i)
            names_.push_back(common::format(prefix_, i));

    name_views_.clear();
    for (const auto& name : names_)
        name_views_.push_back(name);
}

auto DynamicNames::operator()() const
    -> common::ConstNameSpan
{
    return name_views_;
}

} // namespace gc
