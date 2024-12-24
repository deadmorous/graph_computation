#include "agc_perf/mandelbrot.hpp"
#include "agc_perf/mandelbrot_param.hpp"
#include "agc_perf/canvas.hpp"

#include <common/func_ref.hpp>

#include <cassert>
#include <complex>


namespace agc_perf {
namespace {

using Complex = std::complex<double>;

// ---

struct Context_F final
{
    const Complex* c{};
};

struct Context_Iterator final
{
    Complex z{};
};

struct Context_Counter final
{
    uint64_t n{};
};

template <typename In, typename Out>
struct Context_Scale final
{
    Out factor{};
};

template <typename Value>
struct Context_Replicate_p final
{
    const Value* value{};
};

template <typename Value>
struct Context_Replicate_v final
{
    Value value{};
};

template <typename Value>
struct Context_Threshold final
{
    Value threshold{};
};

struct Grid2dParam final
{
    Rect rect;
    A2<double> resolution;
};

struct Context_Canvas final
{
    Canvas canvas;
    size_t index{};
};

template <typename Value>
struct Context_Source final
{
    Value value{};
};

struct State final
{
    Context_Source<Grid2dParam> grid_param;
    Context_Source<Complex> z0;
    Context_Source<uint64_t> max_iter_count;
    Context_Source<double> result_scale_factor;
    Context_Source<double> max_mag2;

    Context_Counter iter_count;
    Context_Replicate_p<Complex> repl_z0;
    Context_F f;
    Context_Iterator f_iter;
    Context_Replicate_p<Complex> repl_iter_val;
    Context_Threshold<uint64_t> threshold_iter_count;
    Context_Threshold<double> threshold_iter_val_mag2;
    Context_Replicate_v<uint64_t> repl_iter_count;
    Context_Scale<uint64_t, double> result_scale;
    Context_Canvas canvas;
};


auto activate_F_p(State* s, const Complex* p)
    -> void;
auto activate_F_arg(State* s, const Complex* arg)
    -> void;
auto activate_Mag2(State* s, const Complex* z)
    -> void;
auto activate_Iterator_init(State* s, const Complex* z0)
    -> void;
auto activate_Iterator_next(State* s)
    -> void;
auto activate_Iterator_f(State* s, const Complex* f_z)
    -> void;
auto activate_Counter_reset(State* s)
    -> void;
auto activate_Counter_next(State* s)
    -> void;
template <typename In, typename Out>
auto activate_Scale_p(State* s, const Out* factor)
    -> void;
template <typename In, typename Out>
auto activate_Scale_scale(State* s, const In* in)
    -> void;
auto activate_Replicate_value_z0(State* s, const Complex* value)
    -> void;
auto activate_Replicate_trigger_z0(State* s)
    -> void;
auto activate_Replicate_value_iter_val(State* s, const Complex* value)
    -> void;
auto activate_Replicate_trigger_iter_val(State* s)
    -> void;
auto activate_Replicate_value_iter_count(State* s, const uint64_t* value)
    -> void;
auto activate_Replicate_trigger_iter_count(State* s)
    -> void;

auto activate_Threshold_threshold_mag2(State* s, const double* threshold)
    -> void;
auto activate_Threshold_check_mag2(State* s, const double* value)
    -> void;
auto activate_Threshold_threshold_iter_count(State* s,
                                             const uint64_t* threshold)
    -> void;
auto activate_Threshold_check_iter_count(State* s, const uint64_t* value)
    -> void;

auto activate_Grid2d_trigger(State* s, const Grid2dParam* param)
    -> void;
auto activate_Canvas_next(State* s, const double* value)
    -> void;
// template <typename Value>
// auto activate_source(Context_Source<Value>* source)
//     -> void;

// ---

auto activate_F_p(State* s, const Complex* p)
    -> void
{ s->f.c = p; }

auto activate_F_arg(State* s, const Complex* arg)
    -> void
{
    auto f_z = *arg * *arg + *s->f.c;
    activate_Iterator_f(s, &f_z);
}

auto activate_Mag2(State* s, const Complex* z)
    -> void
{
    auto mag2 = z->real()*z->real() + z->imag()*z->imag();
    activate_Threshold_check_mag2(s, &mag2);
}

auto activate_Iterator_init(State* s, const Complex* z0)
    -> void
{ s->f_iter.z = *z0; }

auto activate_Iterator_next(State* s)
    -> void
{ activate_F_arg(s, &s->f_iter.z); }

auto activate_Iterator_f(State* s, const Complex* f_z)
    -> void
{
    s->f_iter.z = *f_z;
    activate_Replicate_value_iter_val(s, &s->f_iter.z);
    activate_Counter_next(s);
}

auto activate_Counter_reset(State* s)
    -> void
{ s->iter_count.n = 0; }

auto activate_Counter_next(State* s)
    -> void
{
    ++s->iter_count.n;
    activate_Replicate_value_iter_count(s, &s->iter_count.n);
    activate_Threshold_check_iter_count(s, &s->iter_count.n);
}

auto activate_Scale_p(State* s, const double* factor)
    -> void
{ s->result_scale.factor = *factor; }

auto activate_Scale_scale(State* s, const uint64_t* in)
    -> void
{
    auto result = static_cast<double>(*in * s->result_scale.factor);
    activate_Canvas_next(s, &result);
}

auto activate_Replicate_value_z0(State* s, const Complex* value)
    -> void
{ s->repl_z0.value = value; }

auto activate_Replicate_trigger_z0(State* s)
    -> void
{ activate_Iterator_init(s, s->repl_z0.value); }

auto activate_Replicate_value_iter_val(State* s, const Complex* value)
    -> void
{ s->repl_iter_val.value = value; }

auto activate_Replicate_trigger_iter_val(State* s)
    -> void
{ activate_Mag2(s, s->repl_iter_val.value); }

auto activate_Replicate_value_iter_count(State* s, const uint64_t* value)
    -> void
{ s->repl_iter_count.value = *value; }

auto activate_Replicate_trigger_iter_count(State* s)
    -> void
{ activate_Scale_scale(s, &s->repl_iter_count.value); }

auto activate_Threshold_threshold_mag2(State* s, const double* threshold)
    -> void
{ s->threshold_iter_val_mag2.threshold = *threshold; }

auto activate_Threshold_check_mag2(State* s, const double* value)
    -> void
{
    if (*value < s->threshold_iter_val_mag2.threshold)
        activate_Iterator_next(s);
    else
        activate_Replicate_trigger_iter_count(s);
}

auto activate_Threshold_threshold_iter_count(State* s,
                                             const uint64_t* threshold)
    -> void
{ s->threshold_iter_count.threshold = *threshold; }

auto activate_Threshold_check_iter_count(State* s, const uint64_t* value)
    -> void
{
    if (*value < s->threshold_iter_count.threshold)
        activate_Replicate_trigger_iter_val(s);
    else
        activate_Scale_scale(s, value);
}

auto activate_Grid2d_trigger(State* s, const Grid2dParam* param)
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
            activate_Counter_reset(s);
            activate_Replicate_trigger_z0(s);
            activate_F_p(s, &c);
            activate_Iterator_next(s);
        }
    }
}

auto activate_Canvas_next(State* s, const double* value)
    -> void
{
    s->canvas.canvas.values.at(s->canvas.index) = *value;
    ++s->canvas.index;
}


auto state(const MandelbrotParam& mp)
    -> State
{
    auto result = State{};

    result.grid_param.value =
        Grid2dParam{ .rect = mp.rect, .resolution = mp.resolution };

    auto nx = 1u + static_cast<uint32_t>(mp.rect[0].length() / mp.resolution[0]);
    auto ny = 1u + static_cast<uint32_t>(mp.rect[1].length() / mp.resolution[1]);
    result.canvas.canvas.size = {nx, ny};
    result.canvas.canvas.values.resize(nx*ny);
    result.z0.value = 0;
    result.max_iter_count.value = mp.iter_count;
    result.result_scale_factor.value = 1. / mp.iter_count;
    result.max_mag2.value = mp.magnitude_threshold * mp.magnitude_threshold;
    return result;
}



auto mandelbrot_set(const MandelbrotParam& param)
    -> Canvas
{
    auto s = state(param);

    activate_Replicate_value_z0(&s, &s.z0.value);
    activate_Threshold_threshold_iter_count(&s, &s.max_iter_count.value);
    activate_Scale_p(&s, &s.result_scale_factor.value);
    activate_Threshold_threshold_mag2(&s, &s.max_mag2.value);
    activate_Grid2d_trigger(&s, &s.grid_param.value);

    return s.canvas.canvas;
}

} // anonymous namespace


/** This overload implements an evolution of what's implemented for the
 *  first argument of type `MandelbrotFineGraphImpl3_Tag`. The change done
 *  in this implementatoin is selectively reducing the number of data pointers,
 *  similar to `MandelbrotFineGraphImpl4_Tag`, but this time we prefer to still
 *  use pointers when updates are frequent and data size is greater than the
 *  pointer size.
 *
 *  Benchmarks show 1.47x slowdown with resp. to the reference implementation.
 */
auto mandelbrot_set(MandelbrotFineGraphImpl5_Tag, const MandelbrotParam& param)
    -> Canvas
{ return mandelbrot_set(param); }

} // namespace agc_perf
