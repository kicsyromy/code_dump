#include <cmath>
#include <cstdio>

#include <algorithm>
#include <chrono>
#include <vector>

#include <SFML/Graphics.hpp>
#include <linalg/linalg.h>
#include <sfx/frame_clock.h>

#include "constants.hh"
#include "triangle.hh"
#include "vector.hh"

namespace
{
} // namespace

static void initilize_scene() {}

template <typename draw_vertex_array_function_t>
inline static void update_view(draw_vertex_array_function_t &&draw_vertex_array, float elapsed_time)
{
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
