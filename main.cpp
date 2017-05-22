#include <iostream>

#include <cmath>

#include "application.h"
#include "window.h"
#include "matrix.h"
#include "vertex.h"

using namespace ui;

using engine::vertex;
using engine::matrix3d;

struct line
{
    vertex start;
    vertex end;
};

struct player
{
    vertex coords;
    double angle;
    double fov;
};

static constexpr double PI = std::acos(-1);

constexpr const auto mapWidth  { 600 };
constexpr const auto mapHeight { 800 };

constexpr const auto pFOV { PI / 3 };
constexpr const auto pAngle { 0 };
constexpr const vertex initialPlayerPos {
    mapWidth >> 1,
    (mapWidth >> 1) / std::tan(pFOV / 2)
};

const static line wall { { 100, 100 }, { 500, 100 } };
// In order to maintain the proper FOV we need to position the player in such a way that
// the whole witdth of the projection plane fits into it (FOV). This is the funky calculation
// below for the player's y coordinate
static player p { initialPlayerPos, pAngle, pFOV };

matrix3d transformationMatrix = engine::math::identity<3,3>();

void paint(cairo_t *context, std::size_t width, std::size_t height, void *)
{
    // Fill background
    cairo_set_source_rgb(context, 0.0, 0.0, 0.0);
    cairo_paint(context);

    // Draw "wall"
    // In order to move the world around the player we need to set the transformation origin
    // to the player's fixed position and then rotate and move it back. So the sequence of events
    // are translate -> rotate -> translate
    cairo_set_line_width(context, 3.0);
    cairo_set_source_rgb(context, 1.0, 1.0, 0);
    // Calculate the middle of the segment for the translation
//    const point midPoint  {
//        (wall.start.x + wall.end.x) / 2,
//        (wall.start.y + wall.end.y) / 2
//    };

    const line rotatedWall {
        {
            wall.start.transform(transformationMatrix)
        },
        {
            wall.end.transform(transformationMatrix)
        }
    };
    cairo_move_to(context, rotatedWall.start.x, rotatedWall.start.y);
    cairo_line_to(context, rotatedWall.end.x, rotatedWall.end.y);
    cairo_stroke(context);

    // Draw view angle ray
    static constexpr const int rayLength = 80;
    cairo_set_line_width(context, 5.0);
    cairo_set_source_rgba(context, 1.0, 0, 0, 0.3);
    cairo_move_to(context, initialPlayerPos.x, initialPlayerPos.y);
    // Remember sin() and cos() are always between -1 and 1 and by multiplying by rayLength the result
    // you get the proper coordinates for the ray based on the angle
    cairo_line_to(context, initialPlayerPos.x + std::cos(p.angle) * rayLength, initialPlayerPos.y + std::sin(p.angle) * rayLength);
    cairo_stroke(context);

    // Draw player
    cairo_set_line_width(context, 0.0);
    cairo_set_source_rgb(context, 1.0, 1.0, 1.0);
//    cairo_rectangle(context, p.coords.x - 5, p.coords.y - 5, 10, 10);
    cairo_rectangle(context,  initialPlayerPos.x - 5, initialPlayerPos.y - 5, 10, 10);
    cairo_fill(context);
}

void handleMouseEvent(const MouseEvent &e, void *w)
{
    std::cerr << "Pressed mouse button: " <<
                 (e.b == MouseEvent::Button::Left ? "Left" :
                 e.b == MouseEvent::Button::Right ? "Right" :
                 "Middle") << " at coordinates (" << e.x << ", " << e.y << ")\n";
}

void handleKeyPressEvent(const KeyEvent &e, void *w)
{
    static constexpr const int velocity = 20;
    static constexpr const double rotationVelocity = 0.05;

    const auto sine = std::sin(p.angle);
    const auto cosine = std::cos(p.angle);

    auto deltaX = cosine * velocity;
    auto deltaY = sine * velocity;

    matrix3d translationMatrix {
        { 1, 0, deltaX },
        { 0, 1, deltaY },
        { 0, 0,    1   }
    };

    bool translated = false;

    switch (e.key)
    {
    default:
        break;
    case KeyEvent::Key::Up:
        p.coords.x -= deltaX;
        p.coords.y -= deltaY;
        translated = true;
        break;
    case KeyEvent::Key::Down:
        p.coords.x += deltaX;
        p.coords.y += deltaY;
        translated = true;
        break;
    case KeyEvent::Key::Left:
        p.angle -= rotationVelocity;
        break;
    case KeyEvent::Key::Right:
        p.angle += rotationVelocity;
        break;
    }

    // https://en.wikipedia.org/wiki/Transformation_matrix#Composing_and_inverting_transformations
    matrix3d translateToPlayer {
        { 1, 0, p.coords.x },
        { 0, 1, p.coords.y },
        { 0, 0,      1     }
    };

    matrix3d rotate {
        { cosine,   sine, 0 },
        {  -sine, cosine, 0 },
        {      0,      0, 1 }
    };

    matrix3d translateToOrigin {
        { 1, 0, -p.coords.x },
        { 0, 1, -p.coords.y },
        { 0, 0,      1     }
    };

    transformationMatrix = translationMatrix * translateToPlayer * rotate * translateToOrigin;

    auto window = static_cast<Window *>(w);
    window->repaint();
}

int main(int argc, char *argv[])
{
    f();
    Application app;
    Window w("", mapWidth, mapHeight);
    w.setPaintHandler(&paint, &w);
    w.setMouseHandler(&handleMouseEvent, &w);
    w.setKeyPressHandler(&handleKeyPressEvent, &w);
    w.show();

    return app.exec();
}
