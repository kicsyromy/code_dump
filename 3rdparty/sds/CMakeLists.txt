project (sds)

add_library (
    ${PROJECT_NAME}
	${CMAKE_CURRENT_SOURCE_DIR}/include/sds.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/sdsalloc.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/sds.c
)

target_include_directories (
    ${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options (
	${PROJECT_NAME} PRIVATE
	-std=c99
	-pedantic
)
