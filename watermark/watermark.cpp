
#include "watermark.h"

#include <cmath>
#include <cstdint>

// Смешивает один цветовой канал двух пикселей
static uint8_t BlendChannel(uint8_t original, uint8_t mark, double alpha) {
    double result = original * (1.0 - alpha) + mark * alpha;

    return static_cast<uint8_t>(std::lround(result));
}

bool ApplyWatermark(BMP& base, const BMP& watermark, int offset_x, int offset_y, double alpha) {
    // Проверяем корректность параметров
    if (base.GetWidth() <= 0 ||
        base.GetHeight() <= 0 ||
        watermark.GetWidth() <= 0 ||
        watermark.GetHeight() <= 0 ||
        base.GetBitDepth() != 24 ||
        watermark.GetBitDepth() != 24 ||
        alpha < 0.0 ||
        alpha > 1.0 ||
        !std::isfinite(alpha)) {
        return false;
    }

    // Обходим все пиксели водяного знака
    for (int y = 0; y < watermark.GetHeight(); ++y) {
        for (int x = 0; x < watermark.GetWidth(); ++x) {
            int target_x = offset_x + x;
            int target_y = offset_y + y;

            // Не обрабатываем пиксели за границами оригинала
            if (target_x < 0 ||
                target_y < 0 ||
                target_x >= base.GetWidth() ||
                target_y >= base.GetHeight()) {
                continue;
            }

            const Pixel& mark_pixel =
                watermark.GetPixel(x, y);

            Pixel& original_pixel =
                base.GetPixel(target_x, target_y);

            // Смешиваем каждый цветовой канал отдельно
            original_pixel.red = BlendChannel(
                original_pixel.red,
                mark_pixel.red,
                alpha
            );

            original_pixel.green = BlendChannel(
                original_pixel.green,
                mark_pixel.green,
                alpha
            );

            original_pixel.blue = BlendChannel(
                original_pixel.blue,
                mark_pixel.blue,
                alpha
            );
        }
    }

    return true;
}