#include "test_sequence.hpp"

#include <cassert>


namespace {

auto test_seq(Uint limit)
-> UintVec
{
    assert(limit > 0);
    auto result = UintVec(limit, 1);
    result[0] = 0;
    for (size_t index=0, n=1; index<limit; index+=n, n+=1)
        result[index] = 0;
    return result;
}

}

auto TestSequence::generate(Uint limit)
    -> ConstUintSpan
{
    if (state_.size() < limit)
        state_ = test_seq(limit);

    return {state_.data(), limit};
}
