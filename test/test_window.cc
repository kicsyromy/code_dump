#include <catch2/catch.hpp>

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wkeyword-macro"
#endif
#define private public
#ifdef __clang__
#pragma clang diagnostic pop
#endif

#include "gui/voot_window.hh"
#include "voot_window.cc"

TEST_CASE("Test app with one window", "[window]")
{
    using namespace voot;
    Application app;
    voot::Window window{ "Test Window" };
    app.exec();
}
