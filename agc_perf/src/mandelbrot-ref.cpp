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

#include <complex>


namespace agc_perf {
namespace {

using Complex = std::complex<double>;

auto mandelbrot_set_value(Complex c,
                          size_t iter_count,
                          double factor,
                          double magnitude_threshold_square)
    -> double
{
    auto z = Complex{};
    for (size_t iter=0; iter<iter_count; ++iter)
    {
        z = z*z + c;
        if (z.real()*z.real() + z.imag()*z.imag() >= magnitude_threshold_square)
            return double(iter+1) * factor;
    }
    return 1;
}

auto mandelbrot_set(const MandelbrotParam& param)
    -> Canvas
{
    const auto& [rect, resolution, iter_count, magnitude_threshold] = param;

    auto nx = 1u + static_cast<uint32_t>(rect[0].length() / resolution[0]);
    auto ny = 1u + static_cast<uint32_t>(rect[1].length() / resolution[1]);
    auto result = Canvas
    {
        .size = { nx, ny }
    };
    result.values.reserve(nx*ny);

    uint32_t iy = 0;
    double y = rect[1].begin;
    auto factor = 1. / iter_count;
    auto magnitude_threshold_square = magnitude_threshold * magnitude_threshold;
    for (; iy<ny; ++iy, y+=resolution[1])
    {
        uint32_t ix = 0;
        double x = rect[0].begin;
        for (; ix<nx; ++ix, x+=resolution[0])
        {
            auto c = Complex{x, y};
            result.values.push_back(
                mandelbrot_set_value(
                    c, iter_count, factor, magnitude_threshold_square));
        }
    }

    return result;
}

} // anonymous namespace


/** This overload provides a reference single-threaded straightforward
 *  implementation computing Mandelbrot's set. It's purpose is to provide
 *  a reference execution time and a reference result, that other
 *  implementations are to be compared against. */
auto mandelbrot_set(MandelbrotRefImpl_Tag, const MandelbrotParam& param)
    -> Canvas
{ return mandelbrot_set(param); }

} // namespace agc_perf
