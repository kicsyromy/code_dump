cmake_minimum_required (VERSION 3.15)

project ("Voot" CXX)

option (BUILD_SHARED_LIBS "Enable compilation of shared libraries" OFF)
option (VOOT_BUILD_SHARED "Build Voot as a shared library even if BUILD_SHARED_LIBS is set to OFF" OFF)
option (VOOT_ENABLE_PCH "Enable Precompiled Headers" OFF)
option (VOOT_TREAT_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" ON)

option (VOOT_ENABLE_TESTING "Enable Test Builds" ON)
option (VOOT_ENABLE_COVERAGE "Enable coverage reporting for gcc/clang" FALSE)

include (cmake/CompilerWarnings.cmake)
include (cmake/Doxygen.cmake)
include (cmake/IPO.cmake)
include (cmake/Sanitizers.cmake)
include (cmake/StandardProjectSettings.cmake)
include (cmake/StaticAnalyzers.cmake)

add_library (project_options INTERFACE)
add_library (project_warnings INTERFACE)

target_compile_features (project_options INTERFACE cxx_std_17)
set_project_warnings (project_warnings)
enable_sanitizers (project_options)
enable_doxygen ()

if (VOOT_ENABLE_PCH)
    target_precompile_headers (project_options INTERFACE <vector> <string> <map> <utility>)
endif ()

add_subdirectory (3rdparty)
add_subdirectory (src)

if (VOOT_ENABLE_TESTING)
    enable_testing ()
    message (STATUS "[Voot] Building Tests")
    add_subdirectory (test)
endif ()

if (VOOT_ENABLE_FUZZING)
    message (STATUS "[Voot] Building Fuzz Tests, using fuzzing sanitizer https://www.llvm.org/docs/LibFuzzer.html")
    add_subdirectory (fuzz_test)
endif ()
