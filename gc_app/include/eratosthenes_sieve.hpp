#pragma once

#include "sequence_generator.hpp"


class EratosthenesSieve final
    : public SequenceGeneratorInterface
{
public:
    virtual auto generate(Uint size)
        -> ConstUintSpan override;

private:
    UintVec state_;
};
