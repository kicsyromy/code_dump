#include <cmath>
#include <cstdio>

#include <algorithm>
#include <chrono>
#include <vector>

#include <SFML/Graphics.hpp>
#include <linalg/linalg.h>

#include "file.hh"
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

    static mesh mesh_cube = mesh::load_from_object_file(ASSET_PATH "/space_ship.obj");
    ::mesh_cube = &mesh_cube;

    static vector3f camera{ 0, 0, 0 };
    ::camera = &camera;
}

template <typename draw_vertex_array_function_t>
inline static void update_view(draw_vertex_array_function_t &&draw_vertex_array, float elapsed_time)
{
    using namespace utils;

    static constexpr vector3f light_direction{ 0.f, 0.f, -1.f };

    static auto rotation_angle = 0.f;
    rotation_angle += 1.f * elapsed_time;

    std::vector<triangle> toRasterize;

    for (const auto &triangle : mesh_cube->triangles)
    {
        const auto translated_triangle =
            triangle
                .rotate<Axis::X>(rotation_angle)
                //                                             .rotate<Axis::Y>(rotation_angle * 0.1f)
                .rotate<Axis::Z>(rotation_angle * 0.5f)
                .translate<Axis::Z>(8.0f);

        const auto normal = linalg::normalize(translated_triangle.normal());

        if (linalg::dot(normal, { translated_triangle.vertices[0].x - camera->x,
                                  translated_triangle.vertices[0].y - camera->y,
                                  translated_triangle.vertices[0].z - camera->z }) < 0.f)
        {

            auto projected_triangle = translated_triangle.project(90.f, 1000.f, 0.1f, 1)
                                          .scale(200, 200)
                                          .translate<Axis::X>(SCREEN_WIDTH / 2)
                                          .translate<Axis::Y>(SCREEN_HEIGHT / 2);

            const auto dp = linalg::dot(normal, linalg::normalize(light_direction));
            projected_triangle.color = get_color(dp, sf::Color::Red);

            toRasterize.push_back(projected_triangle);
        }
    }

    std::sort(toRasterize.begin(), toRasterize.end(), [](const auto &t1, const auto &t2) {
        const auto z1 = t1.template mid_point<Axis::Z>();
        const auto z2 = t2.template mid_point<Axis::Z>();

        return z1 > z2;
    });

    for (const auto &t : toRasterize)
    {
        draw_model(draw_vertex_array, t.vertices, sf::TriangleFan, t.color);
        //        draw_model(draw_vertex_array, t.vertices, sf::LineStrip, sf::Color::White);
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
