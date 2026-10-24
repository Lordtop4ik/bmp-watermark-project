#pragma once
#include <cstdint>

struct Pixel {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
};

// Хранит пиксели построчно, начиная с верхнего левого угла.
class BMP {
public:
    BMP();
    ~BMP();

    BMP(const BMP&) = delete;
    BMP& operator=(const BMP&) = delete;

    // Несжатый 24-битный BMP v3. При ошибке прежнее изображение сохраняется.
    bool Load(const char* filename);
    // Записывает полноценный BMP: заголовки, пиксели и выравнивание строк.
    bool Save(const char* filename) const;

    int GetWidth() const;
    int GetHeight() const;
    int GetBitDepth() const;

    // Координаты должны находиться в пределах изображения.
    Pixel& GetPixel(int x, int y);
    const Pixel& GetPixel(int x, int y) const;

private:
    int width;
    int height;
    int bit_depth;
    Pixel* pixels;
};
