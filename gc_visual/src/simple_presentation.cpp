#include "simple_presentation.hpp"

auto SimplePresentation::represent(QImage& img, SequenceGeneratorInterface& gen)
    -> void
{
    uint32_t width = 1500;
    uint32_t height = 1500;

    auto n = width*height;
    auto seq = gen.generate(n);
    QImage result(width, height, QImage::Format_RGB32);

    auto N = *std::max_element(seq.begin(), seq.end());
    auto d = 255 / N;

    uint32_t index = 0;
    for (uint32_t row=0; row<height; ++row)
    {
        auto* pixel = reinterpret_cast<QRgb*>(result.scanLine(row));
        for (uint32_t col=0; col<width; ++col, ++pixel, ++index)
        {
            auto value = seq[index];
            if (value == 0)
                *pixel = qRgb(0, 0, 0);
            else
            {
                // *pixel = qRgb(0xff, 0xff, 0xff);
                auto v = value*d;
                *pixel = qRgb(0xff, 0xff-v, 0xff-v);
            }
        }
    }

    img = result;
}
