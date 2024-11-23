#pragma once

#include "gc/activation_node_fwd.hpp"
#include "gc/algorithm_fwd.hpp"
#include "gc/value_fwd.hpp"

#include "common/const_name_span.hpp"
#include "common/pow2.hpp"

#include <cassert>

namespace gc {

template <size_t> struct uint_of_size;

template <size_t size>
using uint_of_size_t = uint_of_size<size>::type;

template <> struct uint_of_size<8> final { using type = uint8_t; };
template <> struct uint_of_size<16> final { using type = uint16_t; };
template <> struct uint_of_size<32> final { using type = uint32_t; };
template <> struct uint_of_size<64> final { using type = uint64_t; };

template <size_t count_, typename Tag>
class BitSet
{
public:
    static constexpr auto count = count_;

    using Storage = uint_of_size_t<common::ceil2(count)>;

    constexpr BitSet() noexcept = default;

    /* implicit */ BitSet(std::initializer_list<uint8_t> bits) noexcept
    { set(bits); }

    /* implicit */ constexpr BitSet(Storage data) noexcept : data_{data}
    {}

    auto set(std::initializer_list<uint8_t> bits) noexcept
        -> void
    {
        for (auto bit : bits)
            set(bit);
    }

    auto set(uint8_t bit) noexcept
        -> void
    {
        assert(bit < count);
        data_ |= 1 << bit;
    }

    auto clear(std::initializer_list<uint8_t> bits) noexcept
        -> void
    {
        for (auto bit : bits)
            clear(bit);
    }

    auto clear(uint8_t bit) noexcept
        -> void
    {
        assert(bit < count);
        data_ &= ~(1 << bit);
    }

    auto toggle(std::initializer_list<uint8_t> bits) noexcept
        -> void
    {
        for (auto bit : bits)
            toggle(bit);
    }

    auto toggle(uint8_t bit) noexcept
        -> void
    {
        assert(bit < count);
        data_ ^= 1 << bit;
    }

    auto is_set(uint8_t bit) noexcept
        -> bool
    {
        assert(bit < count);
        return (data_ & (1 << count)) != 0;
    }

    constexpr auto raw() const noexcept
    { return data_; }

private:
    Storage data_{};
};

constexpr inline auto max_ports = size_t{8};

struct Ports_Tag final {};

using Ports = BitSet<max_ports, Ports_Tag>;

struct PortActivationAlgorithm final
{
    Ports required_inputs;
    id::Statement activate;
};

struct ActivationNode
{
    virtual ~ActivationNode() = default;

    virtual auto input_names() const -> common::ConstNameSpan = 0;

    virtual auto output_names() const -> common::ConstNameSpan = 0;

    virtual auto default_inputs(gc::ValueSpan result) const -> void = 0;

    virtual auto activation_algorithms(std::span<PortActivationAlgorithm>,
                                       AlgorithmStorage&) const
        -> void = 0;

    auto input_count() const -> uint32_t
    { return input_names().size(); }

    auto output_count() const -> uint32_t
    { return output_names().size(); }
};

} // namespace gc
