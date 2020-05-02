cmake_minimum_required (VERSION 3.15)

project ("Voot" CXX)
include (cmake/StandardProjectSettings.cmake)

add_library (project_options INTERFACE)
target_compile_features (project_options INTERFACE cxx_std_17)

add_library (project_warnings INTERFACE)
include (cmake/CompilerWarnings.cmake)
set_project_warnings (project_warnings)

include (cmake/Sanitizers.cmake)
enable_sanitizers (project_options)

include (cmake/Doxygen.cmake)
enable_doxygen ()

include (cmake/StaticAnalyzers.cmake)

option (BUILD_SHARED_LIBS "Enable compilation of shared libraries" OFF)
option (ENABLE_TESTING "Enable Test Builds" ON)

option (ENABLE_PCH "Enable Precompiled Headers" OFF)
if  (ENABLE_PCH)
  target_precompile_headers (project_options INTERFACE <vector> <string> <map> <utility>)
endif ()

add_subdirectory (3rdparty)

if (ENABLE_TESTING)
  enable_testing ()
  message (
    "Building Tests. Be sure to check out test/constexpr_tests for constexpr testing"
  )
  add_subdirectory (test)
endif ()

if (ENABLE_FUZZING)
  message (
    "Building Fuzz Tests, using fuzzing sanitizer https://www.llvm.org/docs/LibFuzzer.html"
  )
  add_subdirectory (fuzz_test)
endif ()

add_subdirectory (src)
