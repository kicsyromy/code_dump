add_subdirectory (3rdparty)

include (Catch)

add_executable (
    voot_tests
    catch_main.cc
    test_application.cc
    test_binding.cc
    test_event.cc
    test_item.cc
    test_lifeline.cc
    test_logger.cc
    test_property.cc
    test_rectangle.cc
    test_window.cc
)

target_include_directories (
    voot_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/include
)

target_compile_definitions (
    voot_tests PRIVATE
    -DVOOT_TESTING
)

target_link_libraries (
    voot_tests PRIVATE
    project_warnings
    project_options
    Catch2::Catch2
    Microsoft.GSL::GSL
    bgfx::bgfx
    bgfx::nanovg
    SDL2::SDL2
    spdlog::spdlog
)

enable_ipo (voot_tests)

catch_discover_tests(
  voot_tests
  TEST_PREFIX "unittests."
  EXTRA_ARGS
      -s
      --reporter=xml
      --out=tests.xml
)

# Add a file containing a set of constexpr tests
#add_executable(constexpr_tests constexpr_tests.cc)
#target_link_libraries(constexpr_tests PRIVATE project_options project_warnings
#                                              catch_main)
#
#catch_discover_tests(
#  constexpr_tests
#  TEST_PREFIX
#  "constexpr."
#  EXTRA_ARGS
#  -s
#  --reporter=xml
#  --out=constexpr.xml)
#
## Disable the constexpr portion of the test, and build again this allows us to
## have an executable that we can debug when things go wrong with the constexpr
## testing
#add_executable(relaxed_constexpr_tests constexpr_tests.cc)
#target_link_libraries(
#  relaxed_constexpr_tests PRIVATE project_options project_warnings
#                                  catch_main)
#target_compile_definitions(
#  relaxed_constexpr_tests PRIVATE
#                                  -DCATCH_CONFIG_RUNTIME_STATIC_REQUIRE)
#
#catch_discover_tests(
#  relaxed_constexpr_tests
#  TEST_PREFIX
#  "relaxed_constexpr."
#  EXTRA_ARGS
#  -s
#  --reporter=xml
#  --out=relaxed_constexpr.xml)
