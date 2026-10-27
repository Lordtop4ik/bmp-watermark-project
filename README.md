# BMP Watermark Project
[![C++ CI Build](https://github.com/Lordtop4ik/bmp-watermark-project/actions/workflows/ci.yml/badge.svg?branch=integration-ivan-student3)](https://github.com/Lordtop4ik/bmp-watermark-project/actions/workflows/ci.yml)

Командный проект по дисциплине ИСРПО. Программа на C++ для наложения цифрового водяного знака на BMP-изображения.

## Технологии и CI
* C++17
* CMake (кросс-платформенная сборка)
* GitHub Actions (Windows, macOS, Linux)

## Команда проекта
* **Студент 1** — Разработка класса `BMP` (чтение/запись, парсинг заголовков).
* **Студент 2** — Реализация алгоритма наложения (смешивание пикселей, `watermark.cpp`).
* **Студент 3** — Интеграция (`main.cpp`), парсер INI-конфига, настройка CI, автотесты (Python), документация.

## Сборка проекта
Требуется компилятор C++17 (GCC, Clang или MSVC) и утилита `cmake`.

```bash
cmake -B build 
cmake --build build 
```

## Использование

Программа поддерживает 4 режима работы: интерактивный, через INI-файл, через аргументы командной строки и вывод справки.

**1. Запрос помощи (справка):**
```bash
./build/watermark -h