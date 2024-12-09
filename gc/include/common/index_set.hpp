#pragma once

#include "common/detail/index_like.hpp"
#include "common/pow2.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <initializer_list>


namespace common {

namespace detail {
template <size_t> struct uint_of_size;

template <size_t size>
using uint_of_size_t = uint_of_size<size>::type;

template <> struct uint_of_size<8> final { using type = uint8_t; };
template <> struct uint_of_size<16> final { using type = uint16_t; };
template <> struct uint_of_size<32> final { using type = uint32_t; };
template <> struct uint_of_size<64> final { using type = uint64_t; };

} // namespace detail

template <detail::IndexLikeType Index_, size_t count_ = 8*sizeof(Index_)>
class IndexSet
{
public:
    using Index = Index_;
    using Count = detail::IndexDiffType<Index>;
    static constexpr auto raw_count = count_;
    static constexpr auto count = Count{count_};

    using Storage = detail::uint_of_size_t<common::ceil2(raw_count)>;

    constexpr IndexSet() noexcept = default;

    explicit IndexSet(Count n) :
        data_( (Storage{1u} << raw(n)) - 1u )
    { assert(n <= count); }

    /* implicit */ IndexSet(std::initializer_list<Index> indices) noexcept
    { set(indices); }

    auto set(std::initializer_list<Index> indices) noexcept
        -> void
    {
        for (auto index : indices)
            set(index);
    }

    auto set(Index index) noexcept
        -> void
    {
        assert(index < count);
        data_ |= 1 << raw(index);
    }

    auto clear(std::initializer_list<Index> indices) noexcept
        -> void
    {
        for (auto index : indices)
            clear(index);
    }

    auto clear(Index index) noexcept
        -> void
    {
        assert(index < count);
        data_ &= ~(1 << index);
    }

    auto toggle(std::initializer_list<Index> indices) noexcept
        -> void
    {
        for (auto index : indices)
            toggle(index);
    }

    auto toggle(Index index) noexcept
        -> void
    {
        assert(index < count);
        data_ ^= 1 << index;
    }

    auto is_set(Index index) noexcept
        -> bool
    {
        assert(index < count);
        return (data_ & (1 << count)) != 0;
    }

    constexpr auto data() const noexcept
    { return data_; }

private:
    Storage data_{};
};

} // namespace common
