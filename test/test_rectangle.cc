#include <catch2/catch.hpp>

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wkeyword-macro"
#endif
#define private public
#ifdef __clang__
#pragma clang diagnostic pop
#endif

#include "gui/voot_rectangle.hh"
#include "voot_rectangle.cc"

#include "gui/voot_window.hh"

struct Test
{
    void slot(voot::MouseButton button, int x, int y) noexcept
    {
        VT_LOG_INFO("RootItem mouse button pressed B: {} X: {} Y: {}", button, x, y);
    }
};

void free_function(voot::MouseButton button, int x, int y) noexcept
{
    VT_LOG_INFO("Blue Rectangle mouse button pressed B: {} X: {} Y: {}", button, x, y);
}

TEST_CASE("Rectangle", "[rectangle]")
{
    using namespace voot;

    Application app;
    voot::Window window{ "Test Window" };

    Test t;

    window.root_item()->mouse_button_pressed.connect<&Test::slot>(t);
    auto *r = new voot::Rectangle();
    r->mouse_button_pressed.connect<&free_function>();

    auto *r2 = new voot::Rectangle();
    r2->mouse_button_pressed.connect([](MouseButton button, int x, int y) {
        VT_LOG_INFO("Red Rectangle mouse button pressed B: {} X: {} Y: {}", button, x, y);
    });

    auto *r3 = new voot::Rectangle();
    r3->mouse_button_pressed.connect([](MouseButton button, int x, int y) {
        VT_LOG_INFO("Green Rectangle mouse button pressed B: {} X: {} Y: {}", button, x, y);
    });

    // voot::bind(property, expression); // macro??
    // voot::bind(property, property); // method
    // voot::bind(property, function); // method

    r->x = window.root_item()->width() - 140;
    r->y = 20;
    r->width = 100;
    r->height = 50;
    r->set_color(0, 0, 255);

    r2->x = 2;
    r2->y = 2;
    r2->z = 1;
    r2->width = 50;
    r2->height = 20;
    r2->set_color(255, 0, 0);

    r3->set_x(5);
    r3->set_y(5);
    r3->set_z(4);
    r3->set_width(50);
    r3->set_height(20);
    r3->set_color(0, 255, 0);

    r->set_parent_item(window.root_item());
    r2->set_parent_item(r);
    r3->set_parent_item(r);

    app.exec();
}
