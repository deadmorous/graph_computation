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

struct Context_Grid2d final
{
    Complex z{};
};

struct State final
{
    Context_Source<Grid2dParam> grid_param;
    Context_Source<Complex> z0;
    Context_Source<uint64_t> max_iter_count;
    Context_Source<double> result_scale_factor;
    Context_Source<double> max_mag2;

    Context_Grid2d grid;
    Context_Counter iter_count;
    Context_Iterator f_iter;
    Context_Threshold<uint64_t> threshold_iter_count;
    Context_Threshold<double> threshold_iter_val_mag2;
    Context_Scale<uint64_t, double> result_scale;
    Context_Canvas canvas;
};


auto run(State* s) -> void
{
activate_Threshold_threshold_iter_count:
    s->threshold_iter_count.threshold = s->max_iter_count.value;

activate_Scale_p:
    s->result_scale.factor = s->result_scale_factor.value;

activate_Threshold_threshold_mag2:
    s->threshold_iter_val_mag2.threshold = s->max_mag2.value;

activate_Grid2d_trigger:
    {
        const auto& [rect, resolution] = s->grid_param.value;
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
                s->grid.z = Complex{x, y};

                activate_Counter_reset:
                    s->iter_count.n = 0;

                activate_Replicate_trigger_z0:
                activate_Iterator_init:
                    s->f_iter.z = s->z0.value;

                activate_Iterator_next:
                activate_F_arg:

                auto f_z = s->f_iter.z * s->f_iter.z + s->grid.z;

                activate_Iterator_f:
                    s->f_iter.z = f_z;

                activate_Counter_next:
                    ++s->iter_count.n;

                activate_Threshold_check_iter_count:
                    if (s->iter_count.n < s->threshold_iter_count.threshold)
                    {
                        activate_Replicate_trigger_iter_val:

                        activate_Mag2:
                        auto mag2 = s->f_iter.z.real() * s->f_iter.z.real() +
                                    s->f_iter.z.imag() * s->f_iter.z.imag();

                        activate_Threshold_check_mag2:
                        if (mag2 < s->threshold_iter_val_mag2.threshold)
                            goto activate_Iterator_next;
                        else
                        {
                            activate_Replicate_trigger_iter_count:
                            goto activate_Scale_scale;
                        }
                    }
                    else
                        goto activate_Scale_scale;

                activate_Scale_scale:
                    auto result = static_cast<double>(s->iter_count.n * s->result_scale.factor);

                activate_Canvas_next:
                    s->canvas.canvas.values.at(s->canvas.index) = result;
                    ++s->canvas.index;
            }
        }
    }
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

    run(&s);

    return s.canvas.canvas;
}

} // anonymous namespace


/** This overload implements an evolution of what's implemented for the
 *  first argument of type `MandelbrotFineGraphImpl6_Tag`. The change done
 *  in this implementatoin is the elimination of function calls. We attempted
 *  to fuse the entire algorithm into a single function and implement the
 *  control flow using the `goto` statement.
 *
 *  Benchmarks show 1.08x slowdown with resp. to the reference implementation,
 *  which is a little worse than `MandelbrotFineGraphImpl6_Tag`.
 */
auto mandelbrot_set(MandelbrotFineGotoGraphImpl_Tag,
                    const MandelbrotParam& param)
    -> Canvas
{ return mandelbrot_set(param); }

} // namespace agc_perf
