#include "bmp.h"
#include <cstddef>
#include <fstream>
#include <iostream>
#include <limits>
#include <new>

#pragma pack(push, 1)
struct BmpFileHeader {
    uint16_t signature;
    uint32_t file_size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t pixel_offset;
};

struct BmpInfoHeader {
    uint32_t header_size;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bit_depth;
    uint32_t compression;
    uint32_t image_size;
    int32_t x_resolution;
    int32_t y_resolution;
    uint32_t colors_used;
    uint32_t important_colors;
};
#pragma pack(pop)

static_assert(sizeof(BmpFileHeader) == 14);
static_assert(sizeof(BmpInfoHeader) == 40);
static_assert(sizeof(Pixel) == 3);

BMP::BMP() : width(0), height(0), bit_depth(0), pixels(nullptr) {
}

BMP::~BMP() {
    delete[] pixels;
}

bool BMP::Load(const char* filename) {
    if (filename==nullptr){
        std::cerr << "Не указан путь к BMP-файлу\n";
        return false;
    }

    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()){
        std::cerr << "Не удалось открыть BMP-файл: " << filename << '\n';
        return false;
    }

    BmpFileHeader file_header{};
    BmpInfoHeader info_header{};
    file.read((char*)&file_header, sizeof(file_header));
    file.read((char*)&info_header, sizeof(info_header));
    if (!file){
        std::cerr << "Не удалось прочитать заголовки BMP\n";
        return false;
    }

    if (file_header.signature!=0x4D42 || info_header.header_size!=40 ||
        info_header.planes!=1 || info_header.bit_depth!=24 || info_header.compression!=0){
        std::cerr << "Ожидается несжатый 24-битный BMP v3\n";
        return false;
    }
    if (info_header.width<=0 || info_header.height==0 ||
        info_header.height==std::numeric_limits<int32_t>::min()){
        std::cerr << "Некорректные размеры BMP\n";
        return false;
    }
    if (file_header.pixel_offset < sizeof(file_header) + sizeof(info_header)){
        std::cerr << "Некорректное смещение пикселей BMP\n";
        return false;
    }

    int new_width = info_header.width;
    int new_height = info_header.height;
    if (new_height<0){
        new_height = -new_height;
    }
    uint64_t pixel_count = (uint64_t)new_width * new_height;
    uint64_t row_size = (uint64_t)new_width * 3;
    uint64_t padding = (4 - row_size % 4) % 4;
    uint64_t data_size = (row_size + padding) * new_height;
    if (pixel_count > std::numeric_limits<size_t>::max() / sizeof(Pixel) ||
        row_size > (uint64_t)std::numeric_limits<std::streamsize>::max()){
        std::cerr << "Размер BMP слишком велик для загрузки\n";
        return false;
    }

    file.seekg(0, std::ios::end);
    std::streamoff file_size = file.tellg();
    if (!file || file_size<0 || (uint64_t)file_size < file_header.pixel_offset ||
        data_size > (uint64_t)file_size - file_header.pixel_offset){
        std::cerr << "В BMP недостаточно данных для пикселей\n";
        return false;
    }
    file.seekg(file_header.pixel_offset, std::ios::beg);
    if (!file){
        std::cerr << "Не удалось перейти к пикселям BMP\n";
        return false;
    }

    Pixel* new_pixels = nullptr;
    try {
        new_pixels = new Pixel[(size_t)pixel_count];
    } catch (const std::bad_alloc&) {
        std::cerr << "Недостаточно памяти для BMP\n";
        return false;
    }

    for (int i = 0; i<new_height; ++i){
        int y = i;
        if (info_header.height>0){
            y = new_height - 1 - i;
        }
        file.read((char*)(new_pixels + (size_t)y * new_width), (std::streamsize)row_size);

        char padding_bytes[3]{};
        file.read(padding_bytes, (std::streamsize)padding);
        if (!file){
            delete[] new_pixels;
            std::cerr << "Не удалось прочитать пиксели BMP\n";
            return false;
        }
    }

    delete[] pixels;
    pixels = new_pixels;
    width = new_width;
    height = new_height;
    bit_depth = info_header.bit_depth;
    return true;
}

int BMP::GetWidth() const {
    return width;
}

int BMP::GetHeight() const {
    return height;
}

int BMP::GetBitDepth() const {
    return bit_depth;
}

Pixel& BMP::GetPixel(int x, int y) {
    return pixels[(size_t)y * width + x];
}

const Pixel& BMP::GetPixel(int x, int y) const {
    return pixels[(size_t)y * width + x];
}
