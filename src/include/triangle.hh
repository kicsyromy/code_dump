#ifndef TRIANGLE_HH
#define TRIANGLE_HH

#include <array>

#include <SFML/System.hpp>

#include "utils.hh"
#include "vector.hh"

struct triangle
{
    using vertex_axis_t = decltype(vector3f::x);

    std::array<vector3f, 3> vertices;
    sf::Color color;

    template <utils::Axis axis_v> constexpr vertex_axis_t &vertex_axis(std::size_t index_v) noexcept
    {
        using namespace utils;

        if constexpr (axis_v == Axis::X)
        {
            return vertices[index_v].x;
        }
        else if constexpr (axis_v == Axis::Y)
        {
            return vertices[index_v].y;
        }
        else
        {
            return vertices[index_v].z;
        }
    }

    template <utils::Axis axis_v>
    constexpr vertex_axis_t vertex_axis(std::size_t index_v) const noexcept
    {
        using namespace utils;

        if constexpr (axis_v == Axis::X)
        {
            return vertices[index_v].x;
        }
        else if constexpr (axis_v == Axis::Y)
        {
            return vertices[index_v].y;
        }
        else
        {
            return vertices[index_v].z;
        }
    }

    inline triangle apply_matrix(const matrix4x4f &mat) const noexcept
    {
        triangle result;

        for (auto i = 0ull; i < vertices.size(); ++i)
        {
            const auto &vertex = vertices[i];
            result.vertices[i] = utils::apply_matrix({ vertex.x, vertex.y, vertex.z, 1.f }, mat);
        }

        return result;
    }

    template <utils::Axis axis_v> inline triangle rotate(const float angle) const noexcept
    {
        return apply_matrix(utils::rotation_matrix<axis_v>(angle));
    }

    inline triangle scale(const float scale_x, const float scale_y, const float scale_z = 1.f) const
        noexcept
    {
        return apply_matrix(utils::scaling_matrix(scale_x, scale_y, scale_z));
    }

    template <utils::Axis axis_v> inline triangle translate(const float offset) const noexcept
    {
        const auto axis_translation_matrix = [offset]() {
            using namespace utils;

            if constexpr (axis_v == Axis::X)
            {
                return translation_matrix(offset, 0, 0);
            }

            if constexpr (axis_v == Axis::Y)
            {

                return translation_matrix(0, offset, 0);
            }

            if constexpr (axis_v == Axis::Z)
            {

                return translation_matrix(0, 0, offset);
            }
        }();

        return apply_matrix(axis_translation_matrix);
    }

    inline triangle project(const float fov,
                            const float z_far,
                            const float z_near,
                            const float aspect_ratio) const noexcept
    {
        return apply_matrix(utils::projection_matrix(fov, z_far, z_near, aspect_ratio));
    }

    constexpr vector3f normal() const noexcept
    {
        const vector3f line_a{ vertices[1].x - vertices[0].x, vertices[1].y - vertices[0].y,
                               vertices[1].z - vertices[0].z };

        const vector3f line_b{ vertices[2].x - vertices[0].x, vertices[2].y - vertices[0].y,
                               vertices[2].z - vertices[0].z };

        return linalg::cross(line_a, line_b);
    }

    template <utils::Axis axis_v> inline float mid_point() const noexcept
    {
        return (vertex_axis<axis_v>(0) + vertex_axis<axis_v>(1) + vertex_axis<axis_v>(2)) / 3.f;
    }

    std::vector<triangle> clip_against_plane(const vector3f &plane_point,
                                             const vector3f &plane_normal) const noexcept
    {
        std::vector<triangle> result;
        result.reserve(3);

        auto copy = *this;

        const auto normal = linalg::normalize(plane_normal);

        const auto distance = [&plane_point, &normal ](vector3f & point) noexcept
        {
            point = linalg::normalize(point);
            return (normal.x * point.x + normal.y * point.y + normal.z * point.z -
                    linalg::dot(normal, plane_point));
        };

        std::array<vector3f *, 3> inside_points;
        std::size_t inside_point_count{ 0 };

        std::array<vector3f *, 3> outside_points;
        std::size_t outside_point_count{ 0 };

        const std::array<float, 3> distances{ distance(copy.vertices[0]),
                                              distance(copy.vertices[1]),
                                              distance(copy.vertices[2]) };
        for (auto i = 0ull; i < distances.size(); ++i)
        {
            if (distances[i] >= 0)
            {
                inside_points[inside_point_count++] = &copy.vertices[i];
            }
            else
            {
                outside_points[outside_point_count++] = &copy.vertices[i];
            }
        }

        if (inside_point_count == 1 && outside_point_count == 2)
        {
            auto new_triangle = result.emplace_back();
            new_triangle.color = color;

            new_triangle.vertices[0] = *inside_points[0];
            new_triangle.vertices[1] = utils::intersect_with_plane(
                plane_point, normal, *inside_points[0], *outside_points[0]);
            new_triangle.vertices[2] = utils::intersect_with_plane(
                plane_point, normal, *inside_points[0], *outside_points[1]);
        }

        if (inside_point_count == 2 && outside_point_count == 1)
        {
            auto &new_triangle1 = result.emplace_back();
            auto &new_triangle2 = result.emplace_back();

            new_triangle1.color = color;
            new_triangle2.color = color;

            new_triangle1.vertices[0] = *inside_points[0];
            new_triangle1.vertices[1] = *inside_points[1];
            new_triangle1.vertices[2] = utils::intersect_with_plane(
                plane_point, normal, *inside_points[0], *outside_points[0]);

            new_triangle1.vertices[0] = *inside_points[1];
            new_triangle1.vertices[1] = new_triangle1.vertices[2];
            new_triangle1.vertices[2] = utils::intersect_with_plane(
                plane_point, normal, *inside_points[1], *outside_points[0]);
        }

        if (inside_point_count == 3)
        {
            result.push_back(*this);
        }

        return result;
    }
};

#endif /* !TRIANGLE_HH */
