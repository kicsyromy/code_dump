cmake_minimum_required(VERSION 3.14)

set (CMAKE_TOOLCHAIN_FILE cmake/mcs51.cmake)

project (Test C)

add_executable (
    ${PROJECT_NAME}
    main.c
)

add_subdirectory (cpplib)
target_link_libraries (
    ${PROJECT_NAME} PRIVATE
    cpplib
)

add_custom_target (
    packihx ALL
    COMMAND "packihx" "${PROJECT_NAME}.ihx" > "${PROJECT_NAME}.hex"
    USES_TERMINAL
    VERBATIM
)

add_dependencies (
    packihx ${PROJECT_NAME}
)
