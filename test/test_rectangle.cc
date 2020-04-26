#include <catch2/catch.hpp>

#define private public

#include "gui/voot_rectangle.hh"
#include "voot_rectangle.cc"

#include "gui/voot_window.hh"

TEST_CASE("Rectangle", "[rectangle]")
{
    using namespace voot;

    Application app;
    voot::Window window{ "Test Window" };

    auto *r = new voot::Rectangle();
    r->set_parent_item(window.root_item());

    app.exec();
}
