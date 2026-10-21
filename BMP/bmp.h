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

    bool Load(const char* filename);
    bool Save(const char* filename) const;
    // Записывает только заголовки, без пикселей. Файл пока не является полным BMP.
    bool SaveHeaders(const char* filename) const;

    int GetWidth() const;
    int GetHeight() const;
    int GetBitDepth() const;

    // Координаты должны находиться в пределах изображения.
    Pixel& GetPixel(int x, int y);
    const Pixel& GetPixel(int x, int y) const;

private:
    bool SaveImpl(const char* filename, bool write_pixels) const;

    int width;
    int height;
    int bit_depth;
    Pixel* pixels;
};
