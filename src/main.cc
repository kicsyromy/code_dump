#include <cmath>
#include <cstdio>

#include <algorithm>
#include <chrono>
#include <vector>

#include <SFML/Graphics.hpp>
#include <linalg/linalg.h>
#include <sfx/frame_clock.h>

#include "constants.hh"
#include "matrix.hh"
#include "mesh.hh"
#include "triangle.hh"
#include "utils.hh"
#include "vector.hh"

namespace
{
} // namespace

inline mesh *mesh_cube = nullptr;
inline float rotation_angle = 0.f;

static void initilize_scene()
{
    using v3f = vector3f;

    static mesh mesh_cube{ {

        // SOUTH
        { { -0.5f, -0.5f, -0.5f }, { -0.5f, 0.5f, -0.5f }, { 0.5f, 0.5f, -0.5f } },
        { { -0.5f, -0.5f, -0.5f }, { 0.5f, 0.5f, -0.5f }, { 0.5f, -0.5f, -0.5f } },

        // EAST
        { { 0.5f, -0.5f, -0.5f }, { 0.5f, 0.5f, -0.5f }, { 0.5f, 0.5f, 0.5f } },
        { { 0.5f, -0.5f, -0.5f }, { 0.5f, 0.5f, 0.5f }, { 0.5f, -0.5f, 0.5f } },

        // NORTH
        { { 0.5f, -0.5f, 0.5f }, { 0.5f, 0.5f, 0.5f }, { -0.5f, 0.5f, 0.5f } },
        { { 0.5f, -0.5f, 0.5f }, { -0.5f, 0.5f, 0.5f }, { -0.5f, -0.5f, 0.5f } },

        // WEST
        { { -0.5f, -0.5f, 0.5f }, { -0.5f, 0.5f, 0.5f }, { -0.5f, 0.5f, -0.5f } },
        { { -0.5f, -0.5f, 0.5f }, { -0.5f, 0.5f, -0.5f }, { -0.5f, -0.5f, -0.5f } },

        // TOP
        { { -0.5f, 0.5f, -0.5f }, { -0.5f, 0.5f, 0.5f }, { 0.5f, 0.5f, 0.5f } },
        { { -0.5f, 0.5f, -0.5f }, { 0.5f, 0.5f, 0.5f }, { 0.5f, 0.5f, -0.5f } },

        // BOTTOM
        { { 0.5f, -0.5f, 0.5f }, { -0.5f, -0.5f, 0.5f }, { -0.5f, -0.5f, -0.5f } },
        { { 0.5f, -0.5f, 0.5f }, { -0.5f, -0.5f, -0.5f }, { 0.5f, -0.5f, -0.5f } }

    } };
    ::mesh_cube = &mesh_cube;
}

template <typename draw_vertex_array_function_t>
inline static void update_view(draw_vertex_array_function_t &&draw_vertex_array, float elapsed_time)
{
    const auto z_near = 0.1f;
    const auto z_far = 1000.f;
    const auto fov = 90.f;
    const auto aspect_ratio = ASPECT_RATIO;

    rotation_angle += 1.f * elapsed_time;

    const auto z_rotation_matrix = rotation_matrix<vector3f::Z>(rotation_angle);
    const auto x_rotation_matrix = rotation_matrix<vector3f::X>(rotation_angle * 0.5f);
    const auto projection_matrix_v = projection_matrix(fov, z_far, z_near, aspect_ratio);

    for (const auto &t : mesh_cube->triangles)
    {
        const triangle z_rotated_triangle{
            utils::multiply_and_normalize(z_rotation_matrix, t.vertices[0]),
            utils::multiply_and_normalize(z_rotation_matrix, t.vertices[1]),
            utils::multiply_and_normalize(z_rotation_matrix, t.vertices[2])
        };

        const triangle x_rotated_triangle{
            utils::multiply_and_normalize(x_rotation_matrix, z_rotated_triangle.vertices[0]),
            utils::multiply_and_normalize(x_rotation_matrix, z_rotated_triangle.vertices[1]),
            utils::multiply_and_normalize(x_rotation_matrix, z_rotated_triangle.vertices[2])
        };

        triangle translated_triangle = x_rotated_triangle;
        translated_triangle.vertices[0].z() += 3.f;
        translated_triangle.vertices[1].z() += 3.f;
        translated_triangle.vertices[2].z() += 3.f;

        triangle projected_triangle{
            utils::multiply_and_normalize(projection_matrix_v, translated_triangle.vertices[0]),
            utils::multiply_and_normalize(projection_matrix_v, translated_triangle.vertices[1]),
            utils::multiply_and_normalize(projection_matrix_v, translated_triangle.vertices[2])
        };

        projected_triangle.vertices[0].x() += 1.f;
        projected_triangle.vertices[0].y() += 1.f;
        projected_triangle.vertices[1].x() += 1.f;
        projected_triangle.vertices[1].y() += 1.f;
        projected_triangle.vertices[2].x() += 1.f;
        projected_triangle.vertices[2].y() += 1.f;

        projected_triangle.vertices[0].x() *= 0.5f * SCREEN_WIDTH;
        projected_triangle.vertices[0].y() *= 0.5f * SCREEN_HEIGHT;
        projected_triangle.vertices[1].x() *= 0.5f * SCREEN_WIDTH;
        projected_triangle.vertices[1].y() *= 0.5f * SCREEN_HEIGHT;
        projected_triangle.vertices[2].x() *= 0.5f * SCREEN_WIDTH;
        projected_triangle.vertices[2].y() *= 0.5f * SCREEN_HEIGHT;

        utils::draw_model(draw_vertex_array, projected_triangle.vertices);
    }
}

int main()
{
    std::srand(static_cast<unsigned int>(time(nullptr)));

    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), PROJECT_NAME);
    auto view = window.getDefaultView();
    view.rotate(180);
    window.setView(view);
    window.setFramerateLimit(60);

    std::array<char, 256> window_title;
    window_title.fill(0);

    sfx::FrameClock frame_clock;
    frame_clock.setSampleDepth(100);

    initilize_scene();

    sf::Clock timer;
    while (window.isOpen())
    {
        const auto elapsed = timer.restart();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }

            if (event.type == sf::Event::KeyReleased)
            {
                switch (event.key.code)
                {
                    case sf::Keyboard::Escape:
                    case sf::Keyboard::Q:
                        window.close();
                        break;
                    case sf::Keyboard::Space:
                    {
                    }
                    break;
                }
            }
        }

        frame_clock.beginFrame();

        window.clear(sf::Color::Black);

        update_view([&](const sf::Vertex *shape, std::size_t count,
                        sf::PrimitiveType type) { window.draw(shape, count, type); },
                    elapsed.asSeconds());

        window.display();

        frame_clock.endFrame();

        std::snprintf(window_title.data(), 255, PROJECT_NAME " | FPS: %.2f | Frame Time: %.2f",
                      static_cast<double>(frame_clock.getFramesPerSecond()),
                      static_cast<double>(frame_clock.getLastFrameTime().asSeconds() * 1000));
        window.setTitle(window_title.data());
    }

    return 0;
}
