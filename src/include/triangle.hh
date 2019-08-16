#ifndef TRIANGLE_HH
#define TRIANGLE_HH

#include <array>

#include <SFML/Graphics.hpp>
#include <linalg/linalg.h>

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

    constexpr vector3f normal() const noexcept
    {
        const linalg::vec<float, 3> line_a{ vertices[1].x() - vertices[0].x(),
                                            vertices[1].y() - vertices[0].y(),
                                            vertices[1].z() - vertices[0].z() };

        const linalg::vec<float, 3> line_b{ vertices[2].x() - vertices[0].x(),
                                            vertices[2].y() - vertices[0].y(),
                                            vertices[2].z() - vertices[0].z() };

        const auto result = linalg::cross(line_a, line_b);

        return { result.x, result.y, result.z };
    }
};

#endif /* !TRIANGLE_HH */
