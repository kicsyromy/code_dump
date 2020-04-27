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
    auto *r2 = new voot::Rectangle();
    auto *r3 = new voot::Rectangle();
    r->set_parent_item(window.root_item());

    r->set_x(window.root_item()->width() - 140);
    r->set_y(20);
    r->set_width(100);
    r->set_height(50);
    r->set_color(0, 0, 255);

    r2->set_x(2);
    r2->set_y(2);
    r2->set_z(1);
    r2->set_width(50);
    r2->set_height(20);
    r2->set_color(255, 0, 0);

    r3->set_x(5);
    r3->set_y(5);
    r3->set_width(50);
    r3->set_height(20);
    r3->set_color(0, 255, 0);

    r2->set_parent_item(r);
    r3->set_parent_item(r);

    app.exec();
}
