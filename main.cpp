#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <sstream>
#include <vector>
#include "bmp.h"
#include "watermark.h"

// Простой парсер INI-файлов
std::map<std::string, std::string> ParseINI(const std::string& filename) {
    std::map<std::string, std::string> config;
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Не удалось открыть конфигурационный файл: " + filename);
    }

    std::string line;
    while (std::getline(file, line)) {
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        if (line.empty() || line[0] == '#' || line[0] == ';') continue;

        size_t delimiter_pos = line.find('=');
        if (delimiter_pos != std::string::npos) {
            std::string key = line.substr(0, delimiter_pos);
            std::string value = line.substr(delimiter_pos + 1);
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            config[key] = value;
        }
    }
    return config;
}

void PrintHelp(const char* prog_name) {
    std::cout << "Использование:\n";
    std::cout << "  1. Интерактивный режим: " << prog_name << "\n";
    std::cout << "  2. Запуск через INI-файл: " << prog_name << " <путь_к_config.ini>\n";
    std::cout << "  3. Запуск через аргументы: " << prog_name << " <in.bmp> <mark.bmp> <out.bmp> <alpha> <x> <y>\n";
    std::cout << "  4. Справка: " << prog_name << " -h\n\n";
    std::cout << "Пример через аргументы: " << prog_name << " original.bmp mark.bmp out.bmp 0.4 10 10\n";
    std::cout << "Пример через INI: " << prog_name << " config.ini\n";
}

int main(int argc, char* argv[]) {
    std::string input_path, watermark_path, output_path;
    double alpha = 0.4;
    int offset_x = 0, offset_y = 0;

    // --- РЕЖИМ 1: Запрос помощи ---
    if (argc == 2 && (std::string(argv[1]) == "-h" || std::string(argv[1]) == "--help")) {
        PrintHelp(argv[0]);
        return 0;
    }

    // --- РЕЖИМ 2: Чтение из INI-файла ---
    if (argc == 2) {
        std::string config_path = argv[1];
        try {
            std::cout << "[INFO] Загрузка конфигурации: " << config_path << "...\n";
            auto cfg = ParseINI(config_path);
            
            input_path = cfg.count("input_path") ? cfg["input_path"] : "";
            watermark_path = cfg.count("watermark_path") ? cfg["watermark_path"] : "";
            output_path = cfg.count("output_path") ? cfg["output_path"] : "";
            
            if (cfg.count("alpha")) alpha = std::stod(cfg["alpha"]);
            if (cfg.count("offset_x")) offset_x = std::stoi(cfg["offset_x"]);
            if (cfg.count("offset_y")) offset_y = std::stoi(cfg["offset_y"]);
        } catch (const std::exception& e) {
            std::cerr << "[ERROR] " << e.what() << "\n";
            return 1;
        }
    }
    // --- РЕЖИМ 3: Аргументы командной строки ---
    else if (argc == 7) {
        input_path = argv[1];
        watermark_path = argv[2];
        output_path = argv[3];
        try {
            alpha = std::stod(argv[4]);
            offset_x = std::stoi(argv[5]);
            offset_y = std::stoi(argv[6]);
        } catch (const std::exception&) {
            std::cerr << "[ERROR] Ошибка парсинга числовых аргументов.\n";
            return 1;
        }
    }
    // --- РЕЖИМ 4: Интерактивный ввод ---
    else if (argc == 1) {
        std::cout << "=== Интерактивный режим ===\n";
        std::cout << "Введите путь к исходному изображению: ";
        std::getline(std::cin, input_path);
        std::cout << "Введите путь к водяному знаку: ";
        std::getline(std::cin, watermark_path);
        std::cout << "Введите путь для сохранения результата: ";
        std::getline(std::cin, output_path);

        std::string temp;
        std::cout << "Введите прозрачность (alpha, Enter = 0.4): ";
        std::getline(std::cin, temp);
        if (!temp.empty()) alpha = std::stod(temp);

        std::cout << "Введите смещение по X (Enter = 0): ";
        std::getline(std::cin, temp);
        if (!temp.empty()) offset_x = std::stoi(temp);

        std::cout << "Введите смещение по Y (Enter = 0): ";
        std::getline(std::cin, temp);
        if (!temp.empty()) offset_y = std::stoi(temp);
    }
    // --- НЕВЕРНЫЕ АРГУМЕНТЫ ---
    else {
        std::cerr << "[ERROR] Неверное количество аргументов.\n\n";
        PrintHelp(argv[0]);
        return 1;
    }

    // Проверка, что пути не пустые (если INI-файл был неполным)
    if (input_path.empty() || watermark_path.empty() || output_path.empty()) {
        std::cerr << "[ERROR] Не указаны пути к файлам (input, watermark или output).\n";
        return 1;
    }

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

    // Наложение
    std::cout << "[INFO] Наложение (alpha=" << alpha << ", offset=" << offset_x << "," << offset_y << ")...\n";
    if (!ApplyWatermark(original_image, watermark_image, offset_x, offset_y, alpha)) {
        std::cerr << "[ERROR] Ошибка при наложении (проверьте, что метка не выходит за границы).\n";
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