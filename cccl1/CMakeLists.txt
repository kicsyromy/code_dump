cmake_minimum_required(VERSION 3.0)

project(cccl1)

add_compile_options("-std=c++17")

add_executable(${PROJECT_NAME} "main.cpp" "input.txt")
