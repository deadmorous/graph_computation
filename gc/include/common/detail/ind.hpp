#pragma once

#include "common/strong.hpp"

#include <cstddef>
#include <cstdint>
#include <iostream>


namespace common::detail {

// --- Indentation

GCLIB_STRONG_TYPE(Ind, uint16_t, common::StrongCountFeatures);

constexpr inline size_t tab_size = 2;

auto operator<<(std::ostream& s, Ind ind)
    -> std::ostream&;

auto next(Ind ind)
    -> Ind;

class ScopedInd final
{
public:
    ScopedInd(Ind& ind);
    ~ScopedInd();

private:
    Ind& ind_;
};

} // namespace common::detail
