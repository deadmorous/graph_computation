#pragma once

#include <iterator>


namespace agc_rt {

struct ContextHandle {};

template <typename Context>
class ContextManager final
{
private:
    struct ContextHandle : agc_rt::ContextHandle
    {
        Context context;
    };

    static auto downcast(agc_rt::ContextHandle* context_handle)
        -> ContextHandle*
    { return static_cast<ContextHandle*>(context_handle); }

public:
    static auto create_context() -> agc_rt::ContextHandle*
    { return new ContextHandle{}; }

    static auto delete_context(agc_rt::ContextHandle* context_handle) -> void
    { delete downcast(context_handle); }

    static auto context_from_handle(agc_rt::ContextHandle* context_handle)
        -> Context*
    { return &downcast(context_handle)->context; }
};

} // namespace agc_rt

extern "C" {

auto create_context() -> agc_rt::ContextHandle*;

auto delete_context(agc_rt::ContextHandle*) -> void;

} // extern "C"
