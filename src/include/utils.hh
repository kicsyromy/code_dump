#ifndef UTILS_HH
#define UTILS_HH

#include <cmath>
#include <cstdint>

#include <SFML/Graphics.hpp>
#include <linalg/linalg.h>

#include "file.hh"
#include "matrix.hh"
#include "strings.hh"
#include "vector.hh"

namespace utils
{
    constexpr auto PI = static_cast<float>(M_PI);

    enum class Axis
    {
        X,
        Y,
        Z
    };

    template <typename draw_function_t, std::size_t vertex_count>
    static inline void draw_model(draw_function_t &draw,
                                  const std::array<vector3f, vertex_count> &model,
                                  sf::PrimitiveType shape_type = sf::LineStrip,
                                  sf::Color color = sf::Color::White)
    {
        std::array<sf::Vertex, vertex_count + 1> shape;
        for (auto i = 0ull; i < vertex_count; ++i)
        {
            shape[i] = sf::Vertex{ sf::Vector2f{ model[i].x, model[i].y }, color };
        }
        shape[vertex_count] = sf::Vertex{ sf::Vector2f{ model[0].x, model[0].y }, color };
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

    template <Axis axis_v> constexpr auto rotation_matrix(const float angle) noexcept
    {
        if constexpr (axis_v == Axis::X)
        {
            // clang-format off
            return matrix4x4f {
                { 1,                0,               0, 0 },
                { 0,  std::cos(angle), std::sin(angle), 0 },
                { 0, -std::sin(angle), std::cos(angle), 0 },
                { 0,                0,               0, 1 }
            };
            // clang-format on
        }
        else if constexpr (axis_v == Axis::Y)
        {
            // clang-format off
            return matrix4x4f {
                {  std::cos(angle), 0, std::sin(angle), 0 },
                {                0, 1,               0, 0 },
                { -std::sin(angle), 0, std::cos(angle), 0 },
                {                0, 0,               0, 1 }
            };
            // clang-format on
        }
        else
        {
            // clang-format off
            return matrix4x4f {
                {  std::cos(angle), std::sin(angle), 0, 0 },
                { -std::sin(angle), std::cos(angle), 0, 0 },
                {                0,               0, 1, 0 },
                {                0,               0, 0, 1 }
            };
            // clang-format on
        }
    }

    constexpr auto scaling_matrix(const float scale_x,
                                  const float scale_y,
                                  const float scale_z = 1.f) noexcept
    {
        // clang-format off
        return matrix4x4f {
            { scale_x,       0,       0, 0 },
            {       0, scale_y,       0, 0 },
            {       0,       0, scale_z, 0 },
            {       0,       0,       0, 1 }
        };
        // clang-format on
    }

    constexpr auto translation_matrix(const float offset_x,
                                      const float offset_y,
                                      const float offset_z) noexcept
    {
        // clang-format off
        return matrix4x4f {
            {        1,        0,        0, 0 },
            {        0,        1,        0, 0 },
            {        0,        0,        1, 0 },
            { offset_x, offset_y, offset_z, 1 }
        };
        // clang-format on
    }

    constexpr auto projection_matrix(const float fov,
                                     const float z_far,
                                     const float z_near,
                                     const float aspect_ratio) noexcept
    {
        const float projection_function_v = 1.f / std::tan(fov * 0.5f / 180.f * PI);
        const float fov_function_v = z_far / (z_far - z_near);

        // clang-format off
        return matrix4x4f {
            { aspect_ratio * projection_function_v ,                      0,                            0, 0 },
            {                                      0, projection_function_v,                            0, 0 },
            {                                      0,                     0,               fov_function_v, 1 },
            {                                      0,                     0, -1 * z_near * fov_function_v, 0 }
        };
        // clang-format on
    }

    inline auto apply_matrix(const vector4f &coordinates, const matrix4x4f &matrix) noexcept
    {
        const auto result = matrix * coordinates;
        if (result[3] != 0.f)
        {
            return vector3f{ result[0] / result[3], result[1] / result[3], result[2] / result[3] };
        }
        else
        {
            return vector3f{ 0, 0, 0 };
        }
    };
} // namespace utils

#endif /* !UTILS_HH */
