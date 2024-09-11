#pragma once

#include "presentation.hpp"

class SpiralPresentation
    : public PresentationInterface
{
public:
    auto represent(QImage&, SequenceGeneratorInterface&)
        -> void override;
};
