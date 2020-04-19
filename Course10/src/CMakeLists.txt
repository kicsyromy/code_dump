string (REPLACE " " "_" PROJ_EXE_NAME ${PROJECT_NAME})
string (TOLOWER ${PROJ_EXE_NAME} PROJ_EXE_NAME)

add_executable (
    ${PROJ_EXE_NAME}
    application.cc      include/application.hh
    mesh.cc             include/mesh.hh
    main.cc
    program.cc          include/program.hh
    shader.cc           include/shader.hh
                        include/signal.hh
    window.cc           include/window.hh

    shaders/vertex.glsl
    shaders/fragment.glsl
)

target_include_directories (
    ${PROJ_EXE_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_compile_definitions (
    ${PROJ_EXE_NAME} PRIVATE
    -DSOURCE_DIR="${PROJECT_SOURCE_DIR}"
    -DBINARY_DIR="${PROJECT_BINARY_DIR}"
    -DSHADER_DIR="${CMAKE_CURRENT_SOURCE_DIR}/shaders"
    -DAPPLICATION_NAME="${PROJECT_NAME}"
)

target_link_libraries (
    ${PROJ_EXE_NAME} PRIVATE
    project_options
    project_warnings
    spdlog::spdlog
    fmt
    docopt_s
    gl-glew
    glm
    SDL2-static
)
