string (REPLACE " " "_" BINARY_NAME ${PROJECT_NAME})
string (TOLOWER ${BINARY_NAME} BINARY_NAME)

if (VOOT_BUILD_SHARED)
    add_library (${BINARY_NAME} SHARED)
else ()
    add_library (${BINARY_NAME})
endif ()

target_sources(
    ${BINARY_NAME} PRIVATE
    voot_application.cc     include/core/voot_application.hh
    voot_binding.cc         include/core/voot_binding.hh
    voot_lifeline.cc        include/core/voot_lifeline.hh
    voot_logger.cc          include/core/voot_logger.hh
                            include/core/voot_logger.inl
                            include/core/voot_property.hh
                            include/core/voot_signal.hh
    voot_item.cc            include/gui/voot_item.hh
    voot_rectangle.cc       include/gui/voot_rectangle.hh
    voot_window.cc          include/gui/voot_window.hh
                            include/events/voot_event.hh
                            include/events/voot_key_events.hh
                            include/events/voot_mouse_events.hh
                            include/events/voot_render_event.hh
                            include/events/voot_window_events.hh
    voot.cc                 include/voot.hh
                            include/voot_global.hh
)

target_include_directories (
    ${BINARY_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_compile_definitions (
    ${BINARY_NAME} PRIVATE
    -D$<UPPER_CASE:${BINARY_NAME}>_LIB
    -DSOURCE_DIR="${PROJECT_SOURCE_DIR}"
    -DBINARY_DIR="${PROJECT_BINARY_DIR}"
    -DLIBRARY_NAME="${BINARY_NAME}"
)

target_link_libraries (
    ${BINARY_NAME} PRIVATE
    project_options
    project_warnings
    bgfx::bgfx
    bgfx::nanovg
    SDL2::SDL2
)

target_link_libraries (
    ${BINARY_NAME} PUBLIC
    Microsoft.GSL::GSL
    spdlog::spdlog
    hedley::hedley
)

enable_ipo (${BINARY_NAME})
