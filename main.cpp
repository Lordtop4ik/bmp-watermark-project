#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <sstream>
#include <cstdlib>
#include "bmp.h"
#include "watermark.h"

// Простой парсер INI-файлов (без внешних библиотек)
std::map<std::string, std::string> ParseINI(const std::string& filename) {
    std::map<std::string, std::string> config;
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Не удалось открыть конфигурационный файл: " + filename);
    }

    std::string line;
    while (std::getline(file, line)) {
        // Удаляем пробелы в начале и конце
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);

        // Пропускаем пустые строки и комментарии
        if (line.empty() || line[0] == '#' || line[0] == ';') continue;

        size_t delimiter_pos = line.find('=');
        if (delimiter_pos != std::string::npos) {
            std::string key = line.substr(0, delimiter_pos);
            std::string value = line.substr(delimiter_pos + 1);

            // Очистка пробелов вокруг ключа и значения
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));

            config[key] = value;
        }
    }
    return config;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Ошибка: неверное количество аргументов.\n";
        std::cerr << "Использование: " << argv[0] << " <путь_к_config.ini>\n";
        std::cerr << "Пример: " << argv[0] << " config/example.ini\n";
        return 1;
    }

    std::string config_path = argv[1];
    std::map<std::string, std::string> cfg;

    try {
        std::cout << "[INFO] Загрузка конфигурации: " << config_path << "...\n";
        cfg = ParseINI(config_path);
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] " << e.what() << "\n";
        return 1;
    }

    // Извлекаем параметры с значениями по умолчанию
    std::string input_path = cfg.count("input_path") ? cfg["input_path"] : "examples/original.bmp";
    std::string watermark_path = cfg.count("watermark_path") ? cfg["watermark_path"] : "examples/watermark.bmp";
    std::string output_path = cfg.count("output_path") ? cfg["output_path"] : "examples/result.bmp";
    
    double alpha = cfg.count("alpha") ? std::stod(cfg["alpha"]) : 0.4;
    int offset_x = cfg.count("offset_x") ? std::stoi(cfg["offset_x"]) : 0;
    int offset_y = cfg.count("offset_y") ? std::stoi(cfg["offset_y"]) : 0;

    // Загрузка изображений
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

    // Наложение
    std::cout << "[INFO] Наложение водяного знака (alpha=" << alpha << ", offset=" << offset_x << "," << offset_y << ")...\n";
    if (!ApplyWatermark(original_image, watermark_image, offset_x, offset_y, alpha)) {
        std::cerr << "[ERROR] Ошибка при наложении водяного знака. Проверьте границы.\n";
        return 1;
    }

    // Сохранение
    std::cout << "[INFO] Сохранение результата в " << output_path << "...\n";
    if (!original_image.Save(output_path.c_str())) { 
        std::cerr << "[ERROR] Не удалось сохранить результат.\n";
        return 1;
    }

    std::cout << "[SUCCESS] Программа успешно завершила работу!\n";
    return 0;
}