#pragma once


namespace agc_perf {

struct Range
{
    double begin;
    double end;

    constexpr auto length() const noexcept -> double
    { return end - begin; }
};

} // namespace agc_perf
