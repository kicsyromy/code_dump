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
    static constexpr auto vertex_count{ 3ull };

    std::array<vector3f, vertex_count> vertices;
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

    inline std::vector<triangle> clip(const vector3f &point_in_plane,
                                      const vector3f &plane_normal) const noexcept
    {
        const auto normal = linalg::normalize(plane_normal.as_v3f());

        const auto distance = [&normal, &point_in_plane ](const auto &point) noexcept
        {
            const auto n_point = linalg::normalize(point);
            return normal.x * n_point.x + normal.y * n_point.y + normal.z * n_point.z -
                   linalg::dot(normal, point_in_plane.as_v3f());
        };

        std::array<const vector3f *, vertex_count> inside_points;
        std::size_t inside_point_count{ 0 };

        std::array<const vector3f *, vertex_count> outside_points;
        std::size_t outside_point_count{ 0 };

        for (auto i = 0ull; i < inside_points.size(); ++i)
        {
            if (distance(vertices[i].as_v3f()) > 0.f)
            {
                inside_points[inside_point_count++] = &vertices[i];
            }
            else
            {
                outside_points[outside_point_count++] = &vertices[i];
            }
        }

        if (inside_point_count == 0)
        {
            return {};
        }

        if (inside_point_count == 1)
        {
            triangle new_t{ *inside_points[0],
                            utils::intersect_with_plane(point_in_plane, normal, *inside_points[0],
                                                        *outside_points[0]),
                            utils::intersect_with_plane(point_in_plane, normal, *inside_points[0],
                                                        *outside_points[1]),
                            color };

            return { std::move(new_t) };
        }

        if (inside_point_count == 2)
        {
            triangle new_t1{ *inside_points[0], *inside_points[1],
                             utils::intersect_with_plane(point_in_plane, normal, *inside_points[0],
                                                         *outside_points[0]),
                             color };

            triangle new_t2{ *inside_points[1], new_t1.vertices[2],
                             utils::intersect_with_plane(point_in_plane, normal, *inside_points[1],
                                                         *outside_points[0]),
                             color };

            return { std::move(new_t1), std::move(new_t2) };
        }

        if (inside_point_count == 3)
        {
            return { *this };
        }

        return {};
    }
};

#endif /* !TRIANGLE_HH */
