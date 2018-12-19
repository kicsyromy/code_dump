cmake_minimum_required (VERSION 3.11)

project (pthread)

add_library (${PROJECT_NAME} INTERFACE)
target_link_libraries (${PROJECT_NAME} INTERFACE -l${PROJECT_NAME})
add_library (3rdparty::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
