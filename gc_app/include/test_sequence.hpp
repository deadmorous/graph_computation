#pragma once

#include "sequence_generator.hpp"


class TestSequence
    : public SequenceGeneratorInterface
{
public:
    virtual auto generate(Uint size)
        -> ConstUintSpan override;

private:
    UintVec state_;
};
