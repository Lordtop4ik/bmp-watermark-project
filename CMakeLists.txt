cmake_minimum_required(VERSION 3.10)
project(BmpWatermarkProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Выбираем флаги в зависимости от компилятора
if(MSVC)
    # Флаги для Windows (MSVC)
    add_compile_options(/W4)
else()
    # Флаги для Linux и macOS (GCC / Clang)
    add_compile_options(-Wall -Wextra -O2)
endif()

add_executable(watermark 
    main.cpp 
    bmp/bmp.cpp 
    watermark/watermark.cpp
)

target_include_directories(watermark PRIVATE 
    bmp
    watermark
)