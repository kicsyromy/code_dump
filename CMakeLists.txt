project (cl-wine CXX)

cmake_minimum_required(VERSION 3.11)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

set (CL_WINE_SOURCES
    main.cc
)

set (CL_WINE_HEADERS)

add_executable (
    ${PROJECT_NAME}
    ${CL_WINE_SOURCES}
    ${CL_WINE_HEADERS}
)

target_link_libraries (
    ${PROJECT_NAME}
    PRIVATE
    stdc++fs
)
