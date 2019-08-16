#ifndef TRIANGLE_HH
#define TRIANGLE_HH

#include <array>

#include <SFML/Graphics.hpp>
#include <linalg/linalg.h>

#include "matrix.hh"
#include "utils.hh"
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

    inline triangle multiply_by(const matrix4x4f &m) const noexcept
    {
        return { vector3f{ linalg::mul(m, vertices[0].as_v4f()) },
                 vector3f{ linalg::mul(m, vertices[1].as_v4f()) },
                 vector3f{ linalg::mul(m, vertices[2].as_v4f()) } };
    }

    inline void multiply_by(const matrix4x4f &m) noexcept
    {
        for (auto i = 0ull; i < vertices.size(); ++i)
        {
            vertices[i] = linalg::mul(m, vertices[i].as_v4f());
        }
    }

    inline triangle normalize_by_w() const noexcept
    {
        return { utils::normalize(vertices[0], vertices[0].w()),
                 utils::normalize(vertices[1], vertices[1].w()),
                 utils::normalize(vertices[2], vertices[2].w()) };
    }

    inline void normalize_by_w() noexcept
    {
        for (auto i = 0ull; i < vertices.size(); ++i)
        {
            vertices[i] = utils::normalize(vertices[i], vertices[i].w());
        }
    }
};

#endif /* !TRIANGLE_HH */
