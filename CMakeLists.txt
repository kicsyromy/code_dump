cmake_minimum_required (VERSION 3.2.0)
project (gulp)
add_definitions (-DGULP_LIB)
# remove this later
add_definitions (-DGULP_NO_IMPORT_SEMANTICS)

## PRIVATE HEADERS ##
include_directories ("${PROJECT_SOURCE_DIR}/include")
file (GLOB_RECURSE GULP_PRIVATE_HEADERS "${PROJECT_SOURCE_DIR}/include/*.h")

## PUBLIC HEADERS ##
include_directories ("${PROJECT_SOURCE_DIR}/src/include")
file (GLOB_RECURSE GULP_PUBLIC_HEADERS "${PROJECT_SOURCE_DIR}/src/include/*.h")

add_custom_target (gulp_headers SOURCES ${GULP_PRIVATE_HEADERS} ${GULP_PUBLIC_HEADERS})

aux_source_directory ("${PROJECT_SOURCE_DIR}/src" GULP_SOURCES)
add_executable (${PROJECT_NAME} ${GULP_SOURCES})
