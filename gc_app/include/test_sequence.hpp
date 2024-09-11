#pragma once

#include <cstdint>
#include <span>
#include <vector>

using Uint = uint32_t;
using UintVec = std::vector<Uint>;
using ConstUintSpan = std::span<const Uint>;

class TestSequence final
{
public:
    auto generate(Uint size)
        -> ConstUintSpan;

private:
    UintVec state_;
};
