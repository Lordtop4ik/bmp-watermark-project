#include <iostream>
#include <string>
#include "bmp.h"

int main(int argc, char* argv[]) {

    if (argc != 4) {
        std::cerr << "Ошибка: неверное количество аргументов.\n";
        std::cerr << "Использование: " << argv[0] << " <исходный.bmp> <водяной_знак.bmp> <результат.bmp>\n";
        return 1;
    }

    std::string input_path = argv[1];
    std::string watermark_path = argv[2];
    std::string output_path = argv[3];

    BMP original_image;
    std::cout << "[INFO] Загрузка исходного изображения: " << input_path << "...\n";
    if (!original_image.Load(input_path.c_str())) {
        std::cerr << "[ERROR] Не удалось загрузить исходное изображение.\n";
        return 1;
    }

    BMP watermark_image;
    std::cout << "[INFO] Загрузка водяного знака: " << watermark_path << "...\n";
    if (!watermark_image.Load(watermark_path.c_str())) {
        std::cerr << "[ERROR] Не удалось загрузить водяной знак.\n";
        return 1;
    }

    std::cout << "[INFO] Размер оригинала: " << original_image.GetWidth() << "x" << original_image.GetHeight() << "\n";
    std::cout << "[INFO] Размер метки: " << watermark_image.GetWidth() << "x" << watermark_image.GetHeight() << "\n";

    if (watermark_image.GetWidth() > original_image.GetWidth() || 
        watermark_image.GetHeight() > original_image.GetHeight()) {
        std::cerr << "[WARN] Водяной знак больше оригинала. Наложение может выйти за границы!\n";
    }

    return 0;
}