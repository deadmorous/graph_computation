#include "agc_perf/canvas.hpp"
#include "agc_perf/mandelbrot.hpp"
#include "agc_perf/mandelbrot_param.hpp"

#include <benchmark/benchmark.h>


namespace agc_perf {
namespace {

template <typename MandelbrotImpl>
static void BM_MandelbrotSet(benchmark::State& state)
{
    constexpr auto tag = MandelbrotImpl{};
    auto param = MandelbrotParam{};

    for (auto _ : state)
    {
        auto canvas = mandelbrot_set(tag, param);
        benchmark::DoNotOptimize(canvas);
    }
}

BENCHMARK(BM_MandelbrotSet<MandelbrotRefImpl_Tag>);
BENCHMARK(BM_MandelbrotSet<MandelbrotFineGraphImpl_Tag>);
BENCHMARK(BM_MandelbrotSet<MandelbrotFineGraphImpl2_Tag>);
BENCHMARK(BM_MandelbrotSet<MandelbrotFineGraphImpl3_Tag>);
BENCHMARK(BM_MandelbrotSet<MandelbrotFineGraphImpl4_Tag>);
BENCHMARK(BM_MandelbrotSet<MandelbrotFineGraphImpl5_Tag>);
BENCHMARK(BM_MandelbrotSet<MandelbrotFineGraphImpl6_Tag>);
BENCHMARK(BM_MandelbrotSet<MandelbrotFineGotoGraphImpl_Tag>);

} // anonymous namespace
} // namespace agc_perf

BENCHMARK_MAIN();
