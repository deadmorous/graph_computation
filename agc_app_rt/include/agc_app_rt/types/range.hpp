#pragma once

#include "common/struct_type_macro.hpp"


namespace agc_app_rt {

template <typename T>
struct Range
{
    double begin;
    double end;

    constexpr auto length() const noexcept -> double
    { return end - begin; }
};

GCLIB_STRUCT_TYPE(Range<double>, begin, end);
GCLIB_STRUCT_TYPE(Range<float>, begin, end);

} // namespace agc_app_rt
