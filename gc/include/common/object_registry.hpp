#pragma once

#include "common/throw.hpp"

#include <cassert>
#include <map>
#include <memory>
#include <string_view>


namespace common {

template <typename Interface, typename... ConstructionArgs>
class ObjectRegsstry
{
public:
    using FactoryFunc = std::shared_ptr<Interface>(*)(ConstructionArgs...);

    using Storage =
        std::map<std::string_view, FactoryFunc>;

    using StorageConstIterator =
        typename Storage::const_iterator;

    auto register_factory(std::string_view name, FactoryFunc factory)
        -> void
    {
        assert(!storage_.contains(name));
        storage_.emplace(name, factory);
    }

    auto maybe_at(std::string_view name) const noexcept
        -> FactoryFunc
    {
        auto it = storage_.find(name);
        return it->second;
    }

    auto at(std::string_view name) const
        -> FactoryFunc
    {
        auto result = maybe_at(name);
        if (result)
            return result;
        throw_<std::domain_error>(
            "No factory is registered for name '", name, "'");
    }

    auto size() const noexcept
    { return storage_.size(); }

    auto begin() const noexcept
        -> StorageConstIterator
    { return storage_.begin(); }

    auto end() const noexcept
        -> StorageConstIterator
    { return storage_.end(); }

private:
    Storage storage_;
};

} // namespace common
