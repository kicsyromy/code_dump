list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Catch2-2.11.3/contrib/")
add_subdirectory (Catch2-2.11.3)
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} CACHE PATH "Global test modules" FORCE)

