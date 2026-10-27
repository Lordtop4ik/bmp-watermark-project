#pragma once
#include "bmp.h"

bool ApplyWatermark(
    //основное изображение
    BMP& base,
    //водяной знак
    const BMP& watermark,
    //координаты верхнего левого угла
    int offset_x,
    int offset_y,
    //доля цвета водяного знака
    double alpha = 0.4
);
