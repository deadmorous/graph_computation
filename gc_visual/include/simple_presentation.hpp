#pragma once

#include "presentation.hpp"

class SimplePresentation
    : public PresentationInterface
{
public:
    auto represent(QImage&, SequenceGeneratorInterface&)
        -> void override;
};
