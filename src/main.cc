#include <cmath>
#include <cstdio>

#include <chrono>
#include <vector>

#include <SFML/Graphics.hpp>

#include "frame_clock.hh"
#include "matrix.hh"
#include "mesh.hh"
#include "triangle.hh"
#include "utils.hh"

#define CURRENT_LINE static_cast<std::size_t>(__LINE__)

namespace
{
    constexpr auto ASPECT_RATIO{ 16.0 / 9.0 };
    constexpr auto ASPECT_RATIO_F{ static_cast<float>(ASPECT_RATIO) };
    constexpr auto SCREEN_HEIGHT{ 480ull };
    constexpr auto SCREEN_WIDTH{ static_cast<std::size_t>(ASPECT_RATIO * SCREEN_HEIGHT) };

    constexpr auto PI = static_cast<float>(M_PI);

    inline mesh *mesh_cube = nullptr;
    inline vector3f *camera = nullptr;

} // namespace

static void initilize_scene()
{
    using v3f = vector3f;

    static mesh mesh_cube{ {
        // clang-format off

        // SOUTH
        { v3f{ { -0.5f, -0.5f, -0.5f } }, v3f{ { -0.5f, 0.5f, -0.5f } },  v3f{ { 0.5f, 0.5f, -0.5f } } },
        { v3f{ { -0.5f, -0.5f, -0.5f } }, v3f{ { 0.5f, 0.5f, -0.5f } },   v3f{ { 0.5f, -0.5f, -0.5f } } },

        // EAST
        { v3f{ { 0.5f, -0.5f, -0.5f } },  v3f{ { 0.5f, 0.5f, -0.5f } },   v3f{ { 0.5f, 0.5f, 0.5f } } },
        { v3f{ { 0.5f, -0.5f, -0.5f } },  v3f{ { 0.5f, 0.5f, 0.5f } },    v3f{ { 0.5f, -0.5f, 0.5f } } },

        // NORTH
        { v3f{ { 0.5f, -0.5f, 0.5f } },   v3f{ { 0.5f, 0.5f, 0.5f } },    v3f{ { -0.5f, 0.5f, 0.5f } } },
        { v3f{ { 0.5f, -0.5f, 0.5f } },   v3f{ { -0.5f, 0.5f, 0.5f } },   v3f{ { -0.5f, -0.5f, 0.5f } } },

        // WEST
        { v3f{ { -0.5f, -0.5f, 0.5f } },  v3f{ { -0.5f, 0.5f, 0.5f } },   v3f{ { -0.5f, 0.5f, -0.5f } } },
        { v3f{ { -0.5f, -0.5f, 0.5f } },  v3f{ { -0.5f, 0.5f, -0.5f } },  v3f{ { -0.5f, -0.5f, -0.5f } } },

        // TOP
        { v3f{ { -0.5f, 0.5f, -0.5f } },  v3f{ { -0.5f, 0.5f, 0.5f } },   v3f{ { 0.5f, 0.5f, 0.5f } } },
        { v3f{ { -0.5f, 0.5f, -0.5f } },  v3f{ { 0.5f, 0.5f, 0.5f } },    v3f{ { 0.5f, 0.5f, -0.5f } } },

        // BOTTOM
        { v3f{ { 0.5f, -0.5f, 0.5f } },   v3f{ { -0.5f, -0.5f, 0.5f } },  v3f{ { -0.5f, -0.5f, -0.5f } } },
        { v3f{ { 0.5f, -0.5f, 0.5f } },   v3f{ { -0.5f, -0.5f, -0.5f } }, v3f{ { 0.5f, -0.5f, -0.5f } } }

        // clang-format on

    } };
    ::mesh_cube = &mesh_cube;

    static vector3f camera{ { 0, 0, 0 } };
    ::camera = &camera;
}

template <typename draw_vertex_array_function_t>
inline static void update_view(draw_vertex_array_function_t &&draw_vertex_array, float elapsed_time)
{
    using namespace utils;

    static constexpr vector3f light_direction{ { 0.f, 0.f, -1.f } };

    static auto rotation_angle = 0.f;
    rotation_angle += 1.f * elapsed_time;

    for (const auto &triangle : mesh_cube->triangles)
    {
        const auto translated_triangle = triangle.rotate<Axis::X>(rotation_angle)
                                             .rotate<Axis::Y>(rotation_angle * 0.3f)
                                             .rotate<Axis::Z>(rotation_angle * 0.5f)
                                             .translate<Axis::Z>(3.0f);

        const auto normal = translated_triangle.normal().normalized();

        if (normal.dot_product({ { translated_triangle.vertices[0].x() - camera->x(),
                                   translated_triangle.vertices[0].y() - camera->y(),
                                   translated_triangle.vertices[0].z() - camera->z() } }) < 0.f)
        {

            const auto dp = normal.dot_product(light_direction.normalized());
            const auto color = get_color(dp, sf::Color::Cyan);

            const auto projected_triangle = translated_triangle.project(90.f, 1000.f, 0.1f, 1)
                                                .scale(40, 40)
                                                .translate<Axis::X>(SCREEN_WIDTH / 2)
                                                .translate<Axis::Y>(SCREEN_HEIGHT / 2);

            draw_model(draw_vertex_array, projected_triangle.vertices, sf::TriangleFan, color);
            //            draw_model(draw_vertex_array, projected_triangle.vertices, sf::LineStrip,
            //                       sf::Color::Black);
        }
    }
}

int main()
{
    std::srand(static_cast<unsigned int>(time(nullptr)));

    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), PROJECT_NAME);
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
