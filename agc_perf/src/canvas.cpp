#include "agc_perf/brightness_char.hpp"
#include "agc_perf/canvas.hpp"


namespace agc_perf {

auto operator<<(std::ostream& s, const Canvas& canvas)
    -> std::ostream&
{
    const auto* d = canvas.values.data();
    for (uint32_t y=0; y<canvas.size[1]; ++y)
    {
        for (uint32_t x=0; x<canvas.size[0]; ++x, ++d)
            s << brightness_char(*d);
        s << '\n';
    }
    return s;
}

} // namespace agc_perf
