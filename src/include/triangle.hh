#ifndef TRIANGLE_HH
#define TRIANGLE_HH

#include <array>

#include <SFML/System.hpp>

#include "utils.hh"
#include "vector.hh"

struct triangle
{
    std::array<vector3f, 3> vertices;
    sf::Color color;

    template <utils::Axis axis_v>
    constexpr vector3f::element_t &vertex_axis(std::size_t index_v) noexcept
    {
        using namespace utils;

        if constexpr (axis_v == Axis::X)
        {
            return vertices[index_v].x();
        }
        else if constexpr (axis_v == Axis::Y)
        {
            return vertices[index_v].y();
        }
        else
        {
            return vertices[index_v].z();
        }
    }

    template <utils::Axis axis_v>
    constexpr vector3f::element_t vertex_axis(std::size_t index_v) const noexcept
    {
        using namespace utils;

        if constexpr (axis_v == Axis::X)
        {
            return vertices[index_v].x();
        }
        else if constexpr (axis_v == Axis::Y)
        {
            return vertices[index_v].y();
        }
        else
        {
            return vertices[index_v].z();
        }
    }

    template <utils::Axis axis_v> inline triangle rotate(const float angle) const noexcept
    {
        using namespace utils;

        triangle result;

        for (auto i = 0ull; i < vertices.size(); ++i)
        {
            const auto &vertex = vertices[i];
            result.vertices[i] = apply_matrix({ { vertex.x(), vertex.y(), vertex.z(), 1.f } },
                                              rotation_matrix<axis_v>(angle));
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
            result.vertices[i] = apply_matrix({ { vertex.x(), vertex.y(), vertex.z(), 1.f } },
                                              scaling_matrix(scale_x, scale_y, scale_z));
        }

        return result;
    }

    template <utils::Axis axis_v> inline triangle translate(const float offset) const noexcept
    {
        triangle result = *this;

        for (auto i = 0ull; i < vertices.size(); ++i)
        {
            result.vertex_axis<axis_v>(i) += offset;
        }

        return result;
    }

    vector3f normal() const noexcept
    {
        const auto lax = vertices[1].elements[0] - vertices[0].elements[0];
        const auto lay = vertices[1].elements[1] - vertices[0].elements[1];
        const auto laz = vertices[1].elements[2] - vertices[0].elements[2];

        const auto lbx = vertices[2].elements[0] - vertices[0].elements[0];
        const auto lby = vertices[2].elements[1] - vertices[0].elements[1];
        const auto lbz = vertices[2].elements[2] - vertices[0].elements[2];

        return utils::cross_product({ { lax, lay, laz } }, { { lbx, lby, lbz } });
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
            result.vertices[i] = apply_matrix({ { vertex.x(), vertex.y(), vertex.z(), 1.f } },
                                              projection_matrix(fov, z_far, z_near, aspect_ratio));
        }

        return result;
    }
};

#endif /* !TRIANGLE_HH */
