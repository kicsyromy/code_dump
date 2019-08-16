#ifndef TRIANGLE_HH
#define TRIANGLE_HH

#include <array>

#include <SFML/Graphics.hpp>

#include "vector.hh"

struct triangle
{
    std::array<vector3f, 3> vertices;
    sf::Color color;

    inline triangle() = default;

    inline triangle(const vector3f &f1,
                    const vector3f &f2,
                    const vector3f &f3,
                    const sf::Color c = sf::Color::White) noexcept
      : vertices{ f1, f2, f3 }, color{ c }
    {
    }

    inline triangle(vector3f &&f1,
                    vector3f &&f2,
                    vector3f &&f3,
                    const sf::Color c = sf::Color::White) noexcept
      : vertices{ std::move(f1), std::move(f2), std::move(f3) }, color{ c }
    {
    }
};

#endif /* !TRIANGLE_HH */
