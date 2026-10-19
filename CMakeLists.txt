cmake_minimum_required(VERSION 3.10)
project(BmpWatermarkProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -O2)

add_executable(watermark main.cpp bmp.cpp)