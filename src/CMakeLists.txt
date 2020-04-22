string (REPLACE " " "_" BINARY_NAME ${PROJECT_NAME})
string (TOLOWER ${BINARY_NAME} BINARY_NAME)

add_library (
    ${BINARY_NAME} SHARED
#    application.cc          include/application.hh
#                            include/signal.hh
#    window.cc               include/window.hh
                            include/events/voot_event.hh
    voot_logger.cc          include/voot_logger.hh
                            include/voot_logger.inl
    voot.cc                 include/voot.hh
                            include/voot_global.hh
)

target_include_directories (
    ${BINARY_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_compile_definitions (
    ${BINARY_NAME} PRIVATE
    -D${BINARY_NAME}_LIB
    -DUSING_GENERATED_CONFIG_H
    -DSDL_MAIN_HANDLED
    -DSOURCE_DIR="${PROJECT_SOURCE_DIR}"
    -DBINARY_DIR="${PROJECT_BINARY_DIR}"
    -DLIBRARY_NAME="${BINARY_NAME}"
)

target_link_libraries (
    ${BINARY_NAME} PRIVATE
    project_options
    project_warnings
    bgfx
    SDL2-static
    Microsoft.GSL::GSL
)

target_link_libraries (
    ${BINARY_NAME} PUBLIC
    spdlog::spdlog
)
