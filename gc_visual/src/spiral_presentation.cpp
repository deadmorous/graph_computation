#include "gc_visual/spiral_presentation.hpp"

#if 0 // TODO
#include <cmath>
#include <numbers>

auto SpiralPresentation::represent(QImage& img, SequenceGeneratorInterface& gen)
    -> void
{
    uint32_t R = 750;
    uint32_t width = 2*R;
    uint32_t height = width;

    constexpr auto zoom = 40.;

    auto n = width*height / (zoom*zoom);
    auto seq = gen.generate(n);
    QImage result(width, height, QImage::Format_RGB32);

    auto N = *std::max_element(seq.begin(), seq.end());
    auto d = 255 / N;

    constexpr auto c = 0.5 / std::numbers::pi * zoom;
    constexpr auto c_2 = 0.5 * c;

    auto arc_length = [&](double phi)
    {
        auto sq_1p_phi2 = sqrt(1 + phi*phi);
        return c_2 * (phi*sq_1p_phi2 + log(sq_1p_phi2 + phi));
    };

    for (uint32_t row=0; row<height; ++row)
    {
        auto y = static_cast<double>(row) - R;
        auto y2 = y*y;

        auto* pixel = reinterpret_cast<QRgb*>(result.scanLine(row));
        for (uint32_t col=0; col<width; ++col, ++pixel)
        {
            auto x = static_cast<double>(col) - R;
            auto x2 = x*x;

            auto r = sqrt(x2 + y2);

            auto phi = atan2(y, x);
            if (phi < 0)
                phi = 2*std::numbers::pi + phi;

            auto dr = c*phi;
            if (r > dr)
                phi += 2*std::numbers::pi*floor((r-dr)/zoom);

            auto s = arc_length(phi);
            auto index_d = s/zoom + 2;
            auto index = static_cast<uint32_t>(index_d);

            int dL = 0;
            if (zoom > 1)
            {
                auto dL1 = fabs(index_d - index - 0.5) * 2;
                auto dL2 = fabs((c*phi - r) / (0.5*zoom) + 1);
                dL = sqrt(dL1*dL1 + dL2*dL2) * 150;
            }

            if (index >= n)
            {
                *pixel = qRgb(0xcc, 0xff, 0xcc);
                continue;
            }

            auto value = seq[index];
            if (value == 0)
                *pixel = qRgb(0, 0, dL);
            else
            {
                // *pixel = qRgb(0xff-dL/2, 0xff-dL/2, 0xff-dL/2);
                auto v = value*d;
                *pixel = qRgb(0xff-dL/2, 0xff-v, 0xff-v);
            }
        }
    }

    img = result;
}
#endif // 0
