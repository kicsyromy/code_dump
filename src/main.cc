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

inline mesh *mesh = nullptr;
inline float rotation_angle = 0.f;
inline vector3f camera{ 0.f, 0.f, 0.f };
inline vector3f look_direction{ 0.f, 0.f, 1.f };
inline matrix4x4f *projection_matrix_v = nullptr;
inline float player_y_rotation = 0.f;

static void initilize_scene()
{
    const auto z_near = 0.1f;
    const auto z_far = 1000.f;
    const auto fov = 90.f;
    const auto aspect_ratio = ASPECT_RATIO;

    static auto projection_matrix_v = projection_matrix(fov, z_far, z_near, aspect_ratio);
    ::projection_matrix_v = &projection_matrix_v;

    static auto space_ship = mesh::load_from_object_file(ASSET_PATH "/axis.obj");
    ::mesh = &space_ship;
}

template <typename draw_vertex_array_function_t>
inline static void update_view(draw_vertex_array_function_t &&draw_vertex_array, float elapsed_time)
{
    {
        using k = sf::Keyboard;

        if (k::isKeyPressed(k::Up))
        {
            camera.y() += 8.f * elapsed_time;
        }

        if (k::isKeyPressed(k::Down))
        {
            camera.y() -= 8.f * elapsed_time;
        }

        if (k::isKeyPressed(k::Left))
        {
            camera.x() += 8.f * elapsed_time;
        }

        if (k::isKeyPressed(k::Right))
        {
            camera.x() -= 8.f * elapsed_time;
        }

        if (k::isKeyPressed(k::A))
        {
            player_y_rotation -= 2.f * elapsed_time;
        }

        if (k::isKeyPressed(k::D))
        {
            player_y_rotation += 2.f * elapsed_time;
        }

        const vector3f forward_vector{ look_direction.as_v3f() * 8.f * elapsed_time };

        if (k::isKeyPressed(k::W))
        {
            camera = camera.as_v3f() + forward_vector.as_v3f();
        }

        if (k::isKeyPressed(k::S))
        {
            camera = camera.as_v3f() - forward_vector.as_v3f();
        }
    }

    const auto z_rotation_matrix = rotation_matrix<vector3f::Z>(rotation_angle);
    const auto x_rotation_matrix = rotation_matrix<vector3f::X>(rotation_angle * 0.5f);
    const auto translation_matrix_v = translation_matrix(0.f, 0.f, 4.f);

    const auto world_matrix =
        linalg::mul(translation_matrix_v, linalg::mul(z_rotation_matrix, x_rotation_matrix));

    vector3f up_vector{ 0.f, 1.f, 0.f };
    vector3f target_vector{ 0.f, 0.f, 1.f };
    const auto camera_rotation_matrix = rotation_matrix<vector3f::Y>(player_y_rotation);
    look_direction = linalg::mul(camera_rotation_matrix, target_vector.as_v4f());
    target_vector = camera.as_v3f() + look_direction.as_v3f();

    const auto camera_matrix = point_at_matrix(camera, target_vector, up_vector);
    const auto view_matrix = linalg::inverse(camera_matrix);

    std::vector<triangle> to_rasterize;

    for (const auto &t : mesh->triangles)
    {
        triangle transformed_triangle = t.multiply_by(world_matrix);

        const vector3f normal{ linalg::normalize(transformed_triangle.normal().as_v3f()) };

        if (linalg::dot(normal.as_v3f(),
                        transformed_triangle.vertices[0].as_v3f() - camera.as_v3f()) < 0.f)
        {
            const vector3f light_direction{ linalg::normalize(
                linalg::vec<float, 3>{ 0.f, 0.f, -1.f }) };
            const auto luminescence = linalg::dot(normal.as_v3f(), light_direction.as_v3f());
            const auto color = utils::get_color(luminescence, sf::Color::Cyan);

            transformed_triangle.color = color;

            const triangle viewed_triangle =
                static_cast<const triangle &>(transformed_triangle).multiply_by(view_matrix);

            /* Project the triangle from 3D to 2D */
            triangle projected_triangle = viewed_triangle.multiply_by(*projection_matrix_v);
            projected_triangle.normalize_by_w();
            projected_triangle.color = color;

            /* Move the triangle into view in 2D space*/
            projected_triangle.vertices[0].x() += 1.f;
            projected_triangle.vertices[0].y() += 1.f;
            projected_triangle.vertices[1].x() += 1.f;
            projected_triangle.vertices[1].y() += 1.f;
            projected_triangle.vertices[2].x() += 1.f;
            projected_triangle.vertices[2].y() += 1.f;

            /* Scale it up based on windows size */
            projected_triangle.vertices[0].x() *= 0.5f * SCREEN_WIDTH;
            projected_triangle.vertices[0].y() *= 0.5f * SCREEN_HEIGHT;
            projected_triangle.vertices[1].x() *= 0.5f * SCREEN_WIDTH;
            projected_triangle.vertices[1].y() *= 0.5f * SCREEN_HEIGHT;
            projected_triangle.vertices[2].x() *= 0.5f * SCREEN_WIDTH;
            projected_triangle.vertices[2].y() *= 0.5f * SCREEN_HEIGHT;

            to_rasterize.push_back(projected_triangle);
        }
    }

    std::sort(
        to_rasterize.begin(), to_rasterize.end(),
        [](const triangle &t1, const triangle &t2) noexcept {
            const float z1 = (t1.vertices[0].z() + t1.vertices[1].z() + t1.vertices[2].z()) / 3.f;
            const float z2 = (t2.vertices[0].z() + t2.vertices[1].z() + t2.vertices[2].z()) / 3.f;

            return z1 > z2;
        });

    for (const auto &t : to_rasterize)
    {
        utils::draw_model(draw_vertex_array, t.vertices, sf::TriangleStrip, t.color);
        utils::draw_model(draw_vertex_array, t.vertices, sf::LineStrip, sf::Color::Black);
    }
}

int main()
{
    std::srand(static_cast<unsigned int>(time(nullptr)));

    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), PROJECT_NAME);
    auto view = window.getDefaultView();
    view.rotate(180);
    window.setView(view);
    //    window.setFramerateLimit(60);

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
