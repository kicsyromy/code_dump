#ifndef UTILS_HH
#define UTILS_HH

#include <cstdint>

#include <array>

#include <SFML/Graphics.hpp>
#include <linalg/linalg.h>

#include "matrix.hh"
#include "vector.hh"

namespace utils
{
    constexpr auto PI = static_cast<float>(M_PI);

    template <typename draw_function_t, std::size_t vertex_count>
    static inline void draw_model(draw_function_t &draw,
                                  const std::array<vector3f, vertex_count> &model,
                                  sf::PrimitiveType shape_type = sf::LineStrip,
                                  sf::Color color = sf::Color::White)
    {
        std::array<sf::Vertex, vertex_count + 1> shape;
        for (auto i = 0ull; i < vertex_count; ++i)
        {
            shape[i] = sf::Vertex{ sf::Vector2f{ model[i].x(), model[i].y() }, color };
        }
        shape[vertex_count] = sf::Vertex{ sf::Vector2f{ model[0].x(), model[0].y() }, color };
        draw(shape.data(), shape.size(), shape_type);
    }

    inline sf::Color get_color(const float luminescence,
                               const sf::Color &base_color = sf::Color::White)
    {
        const auto l = std::clamp(luminescence, 0.f, 1.f);
        return { static_cast<std::uint8_t>(base_color.r * l),
                 static_cast<std::uint8_t>(base_color.g * l),
                 static_cast<std::uint8_t>(base_color.b * l) };
    }

    constexpr vector3f multiply_and_normalize(const matrix4x4f &m, const vector3f &v) noexcept
    {
        auto result = linalg::mul(m, v.as_v4f());

        if (result.w != 0.f)
        {
            result.x /= result.w;
            result.y /= result.w;
            result.z /= result.w;
        }

        return { result.x, result.y, result.z };
    }

    constexpr vector3f normalize(const vector3f &v, const float f) noexcept
    {
        if (f != 0.f)
        {
            return { v.x() / f, v.y() / f, v.z() / f };
        }

        return { v.x(), v.y(), v.z() };
    }

    inline vector3f intersect_with_plane(const vector3f &point_in_plane,
                                         const vector3f &plane_normal,
                                         const vector3f &line_start,
                                         const vector3f &line_end) noexcept
    {
        const auto normal = linalg::normalize(plane_normal.as_v3f());
        const auto plane_dot_product = -linalg::dot(normal, point_in_plane.as_v3f());

        const auto ad = linalg::dot(line_start.as_v3f(), normal);
        const auto bd = linalg::dot(line_end.as_v3f(), normal);

        const auto t = (-plane_dot_product - ad) / (bd - ad);

        const vector3f line_start_end = line_end.as_v3f() - line_start.as_v3f();
        const vector3f line_to_intersect = line_start_end.as_v3f() * t;

        return line_start.as_v3f() + line_to_intersect.as_v3f();
    }
} // namespace utils

#endif /* !UTILS_HH */
