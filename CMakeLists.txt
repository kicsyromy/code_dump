cmake_minimum_required (VERSION 3.4)
project (uwpapp)

## HEADERS ##
set (HEADERS_PATH "${PROJECT_SOURCE_DIR}/src/include")
file (GLOB_RECURSE HEADERS "${HEADERS_PATH}/*.h")
include_directories ("${HEADERS_PATH}")
source_group ("Header Files" FILES ${HEADERS})

## SOURCES ##
aux_source_directory ("${PROJECT_SOURCE_DIR}/src" SOURCES)

## RESOURCES ##
file (GLOB RESOURCES "${PROJECT_SOURCE_DIR}/resources/*")
source_group("Assets" FILES ${RESOURCES})

## APPLICATION MANIFEST ##
list (APPEND RESOURCES "${PROJECT_SOURCE_DIR}/package.appxmanifest")

set_property (SOURCE ${RESOURCES} PROPERTY VS_DEPLOYMENT_CONTENT 1)
set_property (SOURCE ${RESOURCES} PROPERTY VS_DEPLOYMENT_LOCATION "resources")

add_executable (
    ${PROJECT_NAME} ${SOURCES} ${HEADERS} ${RESOURCES} 
)

set_target_properties (
    ${PROJECT_NAME}       PROPERTIES
    COMPILE_FLAGS         "/FI /ZW" # needed for C++/CX (Windows Runtime)
    CXX_STANDARD          14
    CXX_STANDARD_REQUIRED YES
)

