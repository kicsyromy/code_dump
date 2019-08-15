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

    template <utils::Axis axis_v> inline triangle rotate(const float angle) const noexcept
    {
        using namespace utils;

        triangle result;

        for (auto i = 0ull; i < vertices.size(); ++i)
        {
            const auto &vertex = vertices[i];
            result.vertices[i] =
                apply_matrix({ vertex.x, vertex.y, vertex.z, 1.f }, rotation_matrix<axis_v>(angle));
        }

        return result;
    }

    inline triangle scale(const float scale_x, const float scale_y, const float scale_z = 1.f) const
        noexcept
    {
        using namespace utils;

        triangle result;

        for (auto i = 0ull; i < vertices.size(); ++i)
        {
            const auto &vertex = vertices[i];
            result.vertices[i] = apply_matrix({ vertex.x, vertex.y, vertex.z, 1.f },
                                              scaling_matrix(scale_x, scale_y, scale_z));
        }

        return result;
    }

    template <utils::Axis axis_v> inline triangle translate(const float offset) const noexcept
    {
        using namespace utils;

        const auto axis_translation_matrix = [offset]() {
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

        triangle result;

        for (auto i = 0ull; i < vertices.size(); ++i)
        {
            const auto &vertex = vertices[i];
            result.vertices[i] =
                apply_matrix({ vertex.x, vertex.y, vertex.z, 1.f }, axis_translation_matrix);
        }

        return result;
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

    inline triangle project(const float fov,
                            const float z_far,
                            const float z_near,
                            const float aspect_ratio) const noexcept
    {
        using namespace utils;

        triangle result;

        for (auto i = 0ull; i < vertices.size(); ++i)
        {
            const auto &vertex = vertices[i];
            result.vertices[i] = apply_matrix({ vertex.x, vertex.y, vertex.z, 1.f },
                                              projection_matrix(fov, z_far, z_near, aspect_ratio));
        }

        return result;
    }
};

#endif /* !TRIANGLE_HH */
