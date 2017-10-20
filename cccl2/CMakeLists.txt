cmake_minimum_required(VERSION 3.0)

project(cccl2)

add_compile_options("-std=c++17")

add_executable(${PROJECT_NAME} "main.cpp" "level2-eg.txt")
