if (UNIX AND NOT APPLE)

set(OpenGL_GL_PREFERENCE "GLVND" CACHE STRING "Set OpenGL to GL Vendor Neutral library" FORCE)

find_package(OpenGL REQUIRED)
find_package(EXPAT REQUIRED)
find_package(Fontconfig REQUIRED)
find_package(Freetype REQUIRED)
find_package(ICU REQUIRED COMPONENTS uc)
find_package(JPEG REQUIRED)
find_package(PNG REQUIRED)
find_package(ZLIB REQUIRED)
find_package(PkgConfig QUIET REQUIRED)

pkg_search_module(Harfbuzz REQUIRED IMPORTED_TARGET harfbuzz)
pkg_search_module(WEBP REQUIRED IMPORTED_TARGET libwebp)
pkg_search_module(WEBPMUX REQUIRED IMPORTED_TARGET libwebpmux)
pkg_search_module(WEBPDEMUX REQUIRED IMPORTED_TARGET libwebpdemux)

set(SKIA_LIB_PREFIX "lib")
set(SKIA_LIB_SUFFIX ".a")

target_compile_definitions(
    skia INTERFACE
    -DSK_GL
)

target_link_libraries(
    skia INTERFACE
    OpenGL::OpenGL
    OpenGL::GLX
    EXPAT::EXPAT
    Fontconfig::Fontconfig
    Freetype::Freetype
    PkgConfig::Harfbuzz
    ICU::uc
    PkgConfig::WEBP
    PkgConfig::WEBPMUX
    PkgConfig::WEBPDEMUX
    JPEG::JPEG
    PNG::PNG
    ZLIB::ZLIB
)

endif ()
