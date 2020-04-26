#include <catch2/catch.hpp>

#define private public
#include "gui/voot_window.hh"
#include "voot_window.cc"

TEST_CASE("Test app with one window", "[window]")
{
    using namespace voot;
    Application app;
    voot::Window window{ "Test Window" };
    app.exec();
}
