#pragma once

#include <span> // We need a visible definition of std::size
#include <concepts>
#include <type_traits>

namespace gc_app {

template <typename C, typename T>
concept MutableIndexedContainerOf =
    requires(C& c)
    {
        { c[0] } -> std::same_as< std::remove_cvref_t<T>& >;
        std::size(c);
    };

template <typename T, MutableIndexedContainerOf<T> C>
auto inc_multi_index(C& multi_index, const T& n)
    -> bool
{
    auto s = std::size(multi_index);
    for (size_t i=0; i<s; ++i)
    {
        auto& mi = multi_index[s-1-i];
        if (mi+1 < n)
        {
            ++mi;
            return true;
        }
        mi = 0;
    }
    return false;
}

template <typename T, MutableIndexedContainerOf<T> C>
auto inc_multi_index_mono(C& multi_index, const T& n)
    -> bool
{
    auto s = std::size(multi_index);
    for (size_t i=0; i<s; ++i)
    {
        auto& mi = multi_index[s-1-i];
        if (mi+1 < n)
        {
            ++mi;
            for (size_t j=0; j<i; ++j)
                multi_index[s-1-j] = mi;
            return true;
        }
    }
    return false;
}

} // namespace gc_app
