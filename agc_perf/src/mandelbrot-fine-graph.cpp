/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "agc_perf/mandelbrot.hpp"
#include "agc_perf/mandelbrot_param.hpp"
#include "agc_perf/canvas.hpp"

#include <common/func_ref.hpp>

#include <cassert>
#include <complex>
#include <memory>


namespace agc_perf {
namespace {

using Complex = std::complex<double>;

template <typename Arg>
using Activate = common::FuncRef<void(const Arg*)>;

template <typename Context, typename Arg>
auto bind(Activate<Arg>& a, Context* ctx, void(*fn)(Context*, const Arg*))
    -> void
{ a = Activate<Arg>{ fn, ctx }; }

template <typename Context, typename Arg, void(*fn)(Context*, const Arg*)>
auto bind(Activate<Arg>& a, Context* ctx, common::Const_Tag<fn>)
    -> void
{ a = Activate<Arg>{ fn, ctx }; }

template <typename Context, typename Arg, void(*fn)(Context*)>
auto bind(Activate<Arg>& a, Context* ctx, common::Const_Tag<fn>)
    -> void
{
    constexpr auto* adaptor =
        +[](Context* ctx, const Arg*) -> void
    { fn(ctx); };
    a = Activate<Arg>{ adaptor, ctx };
}

// ---

struct Context_F final
{
    const Complex* c{};
    Complex f_z{};
    Activate<Complex> out;
};

auto activate_F_p(Context_F* ctx, const Complex* p)
    -> void
{ ctx->c = p; }

auto activate_F_arg(Context_F* ctx, const Complex* arg)
    -> void
{
    ctx->f_z = *arg * *arg + *ctx->c;
    ctx->out(&ctx->f_z);
}

// ---

struct Context_Mag2 final
{
    double mag2;
    Activate<double> out;
};

auto activate_Mag2(Context_Mag2* ctx, const Complex* z)
    -> void
{
    ctx->mag2 = z->real()*z->real() + z->imag()*z->imag();
    ctx->out(&ctx->mag2);
}

// ---

struct Context_Iterator final
{
    Complex z{};
    Activate<Complex> f;
    Activate<Complex> out;
};

auto activate_Iterator_init(Context_Iterator* ctx, const Complex* z0)
    -> void
{ ctx->z = *z0; }

auto activate_Iterator_next(Context_Iterator* ctx)
    -> void
{ ctx->f(&ctx->z); }

auto activate_Iterator_f(Context_Iterator* ctx, const Complex* f_z)
    -> void
{
    ctx->z = *f_z;
    ctx->out(&ctx->z);
}

// ---

struct Context_Counter final
{
    uint64_t n{};
    Activate<uint64_t> out;
};

auto activate_Counter_reset(Context_Counter* ctx)
    -> void
{ ctx->n = 0; }

auto activate_Counter_next(Context_Counter* ctx)
    -> void
{
    ++ctx->n;
    ctx->out(&ctx->n);
}

// ---

template <typename In, typename Out>
struct Context_Scale final
{
    const Out* factor{};
    Out result{};
    Activate<Out> out;
};

template <typename In, typename Out>
auto activate_Scale_p(Context_Scale<In, Out>* ctx, const Out* factor)
    -> void
{ ctx->factor = factor; }

template <typename In, typename Out>
auto activate_Scale_scale(Context_Scale<In, Out>* ctx, const In* in)
    -> void
{
    ctx->result = static_cast<Out>(*in * *ctx->factor);
    ctx->out(&ctx->result);
}

// ---

template <typename Value>
struct Context_Replicate final
{
    const Value* value{};
    Activate<Value> out;
};

template <typename Value>
auto activate_Replicate_value(Context_Replicate<Value>* ctx, const Value* value)
    -> void
{ ctx->value = value; }

template <typename Value>
auto activate_Replicate_trigger(Context_Replicate<Value>* ctx)
    -> void
{ ctx->out(ctx->value); }

// ---

template <typename Value>
struct Context_Threshold final
{
    const Value *threshold{};
    Activate<Value> check_passed;
    Activate<Value> check_failed;
};

template <typename Value>
auto activate_Threshold_threshold(Context_Threshold<Value>* ctx,
                                  const Value* threshold)
    -> void
{ ctx->threshold = threshold; }

template <typename Value>
auto activate_Threshold_check(Context_Threshold<Value>* ctx,
                              const Value* value)
    -> void
{
    if (*value < *ctx->threshold)
        ctx->check_passed(value);
    else
        ctx->check_failed(value);
}

// ---

template <typename Value, size_t N>
struct Context_Split final
{
    std::array<Activate<Value>, N> out;
};

template <typename Value, size_t N>
auto activate_Split_trigger(Context_Split<Value, N>* ctx, const Value* value)
    -> void
{
    for (const auto& a : ctx->out)
        a(value);
}

// ---

struct Grid2dParam final
{
    Rect rect;
    A2<double> resolution;
};

struct Context_Grid2d final
{
    // TODO: Using A2<double> instead of Complex or using a grid with complex
    // parameters would be more consistent.
    Activate<Complex> out;
};

auto activate_Grid2d_trigger(Context_Grid2d* ctx, const Grid2dParam* param)
    -> void
{
    const auto& [rect, resolution] = *param;
    auto nx = 1u + static_cast<uint32_t>(rect[0].length() / resolution[0]);
    auto ny = 1u + static_cast<uint32_t>(rect[1].length() / resolution[1]);
    uint32_t iy = 0;
    double y = rect[1].begin;
    for (; iy<ny; ++iy, y+=resolution[1])
    {
        uint32_t ix = 0;
        double x = rect[0].begin;
        for (; ix<nx; ++ix, x+=resolution[0])
        {
            auto c = Complex{x, y};
            ctx->out(&c);
        }
    }
}

// ---

struct Context_Canvas final
{
    Canvas canvas;
    size_t index{};
};

auto activate_Canvas_next(Context_Canvas* ctx, const double* value)
    -> void
{
    ctx->canvas.values.at(ctx->index) = *value;
    ++ctx->index;
}

// ---

template <typename Value>
struct Context_Source final
{
    Value value{};
    Activate<Value> out;
};

template <typename Value>
auto activate_source(Context_Source<Value>* source)
    -> void
{ source->out(&source->value); }

// ---

struct State final
{
    Context_Source<Grid2dParam> grid_param;
    Context_Source<Complex> z0;
    Context_Source<uint64_t> max_iter_count;
    Context_Source<double> result_scale_factor;
    Context_Source<double> max_mag2;

    Context_Grid2d grid;
    Context_Split<Complex, 4> split_grid;
    Context_Counter iter_count;
    Context_Replicate<Complex> repl_z0;
    Context_F f;
    Context_Iterator f_iter;
    Context_Split<Complex, 2> split_iter_val;
    Context_Replicate<Complex> repl_iter_val;
    Context_Threshold<uint64_t> threshold_iter_count;
    Context_Mag2 iter_val_mag2;
    Context_Threshold<double> threshold_iter_val_mag2;
    Context_Split<uint64_t, 2> split_iter_count;
    Context_Replicate<uint64_t> repl_iter_count;
    Context_Scale<uint64_t, double> result_scale;
    Context_Canvas canvas;
};

auto state(const MandelbrotParam& mp)
    -> std::unique_ptr<State>
{
    auto result = std::make_unique<State>();

    result->grid_param.value =
        Grid2dParam{ .rect = mp.rect, .resolution = mp.resolution };

    auto nx = 1u + static_cast<uint32_t>(mp.rect[0].length() / mp.resolution[0]);
    auto ny = 1u + static_cast<uint32_t>(mp.rect[1].length() / mp.resolution[1]);
    result->canvas.canvas.size = {nx, ny};
    result->canvas.canvas.values.resize(nx*ny);

    bind(result->grid_param.out, &result->grid, activate_Grid2d_trigger);

    result->z0.value = 0;
    bind(result->z0.out, &result->repl_z0, activate_Replicate_value);

    result->max_iter_count.value = mp.iter_count;
    bind(result->max_iter_count.out,
         &result->threshold_iter_count,
         activate_Threshold_threshold);

    result->result_scale_factor.value = 1. / mp.iter_count;
    bind(result->result_scale_factor.out,
         &result->result_scale,
         activate_Scale_p);

    result->max_mag2.value = mp.magnitude_threshold * mp.magnitude_threshold;
    bind(result->max_mag2.out,
         &result->threshold_iter_val_mag2,
         activate_Threshold_threshold);

    bind(result->grid.out, &result->split_grid, activate_Split_trigger);

    bind(result->split_grid.out[0],
         &result->iter_count,
         common::Const<activate_Counter_reset>);

    bind(result->split_grid.out[1],
         &result->repl_z0,
         common::Const<activate_Replicate_trigger<Complex>>);

    bind(result->split_grid.out[2], &result->f, activate_F_p);

    bind(result->split_grid.out[3],
         &result->f_iter,
         common::Const<activate_Iterator_next>);

    bind(result->iter_count.out, &result->split_iter_count,
         activate_Split_trigger);

    bind(result->repl_z0.out, &result->f_iter, activate_Iterator_init);

    bind(result->f.out, &result->f_iter, activate_Iterator_f);

    bind(result->f_iter.f, &result->f, activate_F_arg);

    bind(result->f_iter.out, &result->split_iter_val, activate_Split_trigger);

    bind(result->split_iter_val.out[0],
         &result->repl_iter_val,
         activate_Replicate_value);

    bind(result->split_iter_val.out[1],
         &result->iter_count,
         common::Const<activate_Counter_next>);

    bind(result->repl_iter_val.out, &result->iter_val_mag2, activate_Mag2);

    bind(result->threshold_iter_count.check_passed,
         &result->repl_iter_val,
         common::Const<activate_Replicate_trigger<Complex>>);

    bind(result->threshold_iter_count.check_failed,
         &result->result_scale,
         activate_Scale_scale);

    bind(result->iter_val_mag2.out,
         &result->threshold_iter_val_mag2,
         activate_Threshold_check);

    bind(result->threshold_iter_val_mag2.check_passed,
         &result->f_iter,
         common::Const<activate_Iterator_next>);

    bind(result->threshold_iter_val_mag2.check_failed,
         &result->repl_iter_count,
         common::Const<activate_Replicate_trigger<uint64_t>>);

    bind(result->split_iter_count.out[0],
         &result->repl_iter_count,
         activate_Replicate_value);

    bind(result->split_iter_count.out[1],
         &result->threshold_iter_count,
         activate_Threshold_check);

    bind(result->repl_iter_count.out,
         &result->result_scale,
         activate_Scale_scale);

    bind(result->result_scale.out,
         &result->canvas,
         activate_Canvas_next);

    return result;
}

auto mandelbrot_set(const MandelbrotParam& param)
    -> Canvas
{
    auto s = state(param);

    activate_source(&s->z0);
    activate_source(&s->max_iter_count);
    activate_source(&s->result_scale_factor);
    activate_source(&s->max_mag2);
    activate_source(&s->grid_param);

    return s->canvas.canvas;
}

} // anonymous namespace


/** This overload provides a fine-graph single-threaded implementation
 *  computing Mandelbrot's set.
 *  Each node activates its output ports using `common::FuncRef` instances
 *  representing activation functions. It looks like the use of these objects
 *  creates an obstacle for code optimizations by compiler, perhaps due to
 *  the fact that `common::FuncRef` implements a kind of type erasure.
 *
 *  Benchmarks show 6.14x slowdown with resp. to the reference implementation.
 */
auto mandelbrot_set(MandelbrotFineGraphImpl_Tag, const MandelbrotParam& param)
    -> Canvas
{ return mandelbrot_set(param); }

} // namespace agc_perf
