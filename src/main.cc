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
} // namespace

inline mesh *mesh_cube = nullptr;
inline vector3f *camera = nullptr;
inline vector3f *look_direction = nullptr;
inline float *player_view_angle = nullptr;

static void initilize_scene()
{
    static mesh mesh_cube = mesh::load_from_object_file(ASSET_PATH "/axis.obj");
    ::mesh_cube = &mesh_cube;

    static vector3f camera{ 0, 0, 0 };
    ::camera = &camera;

    static vector3f look{ 0, 0, 1 };
    ::look_direction = &look;

    static float player_view_angle{ 0.0 };
    ::player_view_angle = &player_view_angle;
}

template <typename draw_vertex_array_function_t>
inline static void update_view(draw_vertex_array_function_t &&draw_vertex_array, float elapsed_time)
{
    using namespace utils;

    static constexpr vector3f light_direction{ 0.f, 0.f, -1.f };

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
    {
        camera->y += 8.0f * elapsed_time;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
    {
        camera->y -= 8.0f * elapsed_time;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
        camera->x += 8.0f * elapsed_time;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        camera->x -= 8.0f * elapsed_time;
    }

    const auto forward = *look_direction * 8.0f * elapsed_time;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        *camera = *camera + forward;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        *camera = *camera - forward;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        *player_view_angle -= 0.01f;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        *player_view_angle += 0.01f;
    }

    static auto rotation_angle = 0.f;
    //    rotation_angle += 1.f * elapsed_time;

    const auto rotation_mat_x = utils::rotation_matrix<Axis::X>(rotation_angle);
    const auto rotation_mat_y = utils::rotation_matrix<Axis::Y>(rotation_angle);
    const auto rotation_mat_z = utils::rotation_matrix<Axis::Z>(rotation_angle);

    const auto translation_mat = utils::translation_matrix(0, 0, 8);
    const auto world_matrix = translation_mat * rotation_mat_x * rotation_mat_z;

    const vector3f up{ 0, 1, 0 };
    vector3f target{ 0, 0, 1 };
    const auto camera_rotation_mat = rotation_matrix<Axis::Y>(*player_view_angle);
    *look_direction = (camera_rotation_mat * vector4f{ target.x, target.y, target.z, 1 }).xyz();
    target = *camera + *look_direction;

    const auto camera_matrix = point_at_matrix(*camera, target, up);
    const auto view_matrix = linalg::inverse(camera_matrix);

    std::vector<triangle> toRasterize;

    for (const auto &triangle : mesh_cube->triangles)
    {
        const auto transformed_triangle = triangle.apply_matrix(world_matrix);

        const auto normal = linalg::normalize(transformed_triangle.normal());

        if (linalg::dot(normal, transformed_triangle.vertices[0] - *camera) < 0.f)
        {
            const auto viewed_triangle = transformed_triangle.apply_matrix(view_matrix);
            auto projected_triangle = viewed_triangle.project(90.f, 1000.f, 0.1f, 1)
                                          .scale(200, 200)
                                          .translate<Axis::X>(SCREEN_WIDTH / 2)
                                          .translate<Axis::Y>(SCREEN_HEIGHT / 2);

            const auto dp = linalg::dot(normal, linalg::normalize(light_direction));
            projected_triangle.color = get_color(dp, sf::Color::White);

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
