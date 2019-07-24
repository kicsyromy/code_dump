cmake_minimum_required(VERSION 3.14)

project (cpplib CXX)

include (../cmake/sdcc_cxx.cmake)

add_library (
    ${PROJECT_NAME}_cxx OBJECT
    test.cpp
)

cbe_precompile_llvm_sources (
    OBJECT_LIBRARY_TARGET ${PROJECT_NAME}_cxx
    NEW_TARGET ${PROJECT_NAME}
    GENERATED_SOURCES ${PROJECT_NAME}_c_sources
)
