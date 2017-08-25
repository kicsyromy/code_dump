cmake_minimum_required (VERSION 2.8)

set (CMAKE_SYSTEM_NAME Generic)
set (CMAKE_SYSTEM_PROCESSOR x86)
set (CMAKE_CROSSCOMPILING 1)

project (DOSTest)
add_executable (${PROJECT_NAME} "main.cpp")
set_property (TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
#set_property (TARGET ${PROJECT_NAME} PROPERTY COMPILE_FLAGS "-S")
