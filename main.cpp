#include <iostream>
#include <string>
#include "bmp.h"
#include "watermark.h"

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
        std::cerr << "[ERROR] Размеры изображений не совпадают: водяной знак больше оригинала!\n";
        return 1;
    }

    std::cout << "[INFO] Наложение водяного знака...\n";
    double alpha = 0.4;
    int offset_x = 0;
    int offset_y = 0;
    
    if (!ApplyWatermark(original_image, watermark_image, offset_x, offset_y, alpha)) {
        std::cerr << "[ERROR] Ошибка при наложении водяного знака.\n";
        return 1;
    }

    std::cout << "[INFO] Сохранение результата в " << output_path << "...\n";
    
    if (!original_image.Save(output_path.c_str())) { 
        std::cerr << "[ERROR] Не удалось сохранить результат.\n";
        return 1;
    }

    std::cout << "[SUCCESS] Программа успешно завершила работу!\n";
    return 0;
}