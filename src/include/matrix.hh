#ifndef MATRIX_HH
#define MATRIX_HH

#include <linalg/linalg.h>

#include "constants.hh"
#include "vector.hh"

using matrix2x2f = linalg::mat<float, 2, 2>;
using matrix3x3f = linalg::mat<float, 3, 3>;
using matrix4x4f = linalg::mat<float, 4, 4>;

template <vector3f::axis axis_v> constexpr auto rotation_matrix(const float angle) noexcept
{
    if constexpr (axis_v == vector3f::X)
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
    else if constexpr (axis_v == vector3f::Y)
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

constexpr auto projection_matrix(const float fov,
                                 const float z_far,
                                 const float z_near,
                                 const float aspect_ratio) noexcept
{
    const float projection_function_v = 1.f / std::tan(fov * 0.5f / 180.f * PI);
    const float z_normalization_f = z_far / (z_far - z_near);

    // clang-format off
    return matrix4x4f {
        { aspect_ratio * projection_function_v ,                      0,                            0, 0 },
        {                                      0, projection_function_v,                            0, 0 },
        {                                      0,                     0,            z_normalization_f, 1 },
        {                                      0,                     0,  -z_near * z_normalization_f, 0 }
    };
    // clang-format on
}

#endif /* !MATRIX_HH */
