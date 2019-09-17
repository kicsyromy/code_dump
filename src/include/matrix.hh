#ifndef MATRIX_HH
#define MATRIX_HH

#include <cmath>

#include <linalg/linalg.h>

using matrix2x2f = linalg::mat<float, 2, 2>;
using matrix3x3f = linalg::mat<float, 3, 3>;
using matrix4x4f = linalg::mat<float, 4, 4>;

namespace matrix
{
    enum class axis
    {
        X,
        Y,
        Z
    };

    template <axis axis_v> constexpr auto rotation(const float angle) noexcept
    {
        if constexpr (axis_v == axis::X)
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
        else if constexpr (axis_v == axis::Y)
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

    constexpr auto scaling(const float scale_x,
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

    constexpr auto translation(const float offset_x,
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

    inline auto point_at(const linalg::vec<float, 3> &position,
                         const linalg::vec<float, 3> &target,
                         const linalg::vec<float, 3> &up) noexcept
    {
        const auto forward = linalg::normalize(target - position);
        const auto newUp = linalg::normalize(up - (forward * linalg::dot(up, forward)));
        const auto right = linalg::cross(newUp, forward);

        // clang-format off
        return matrix4x4f {
            {    right.x,    right.y,    right.z, 0 },
            {    newUp.x,    newUp.y,    newUp.z, 0 },
            {  forward.x,  forward.y,  forward.z, 0 },
            { position.x, position.y, position.z, 1 }
        };
        // clang-format on
    }

    inline auto look_at(const linalg::vec<float, 3> &position,
                        const linalg::vec<float, 3> &target,
                        const linalg::vec<float, 3> &up) noexcept
    {
        return linalg::inverse(point_at(position, target, up));
    }

    inline auto perspective_projection(const float fov,
                                       const float z_near,
                                       const float z_far,
                                       const float aspect_ratio) noexcept
    {
        const float fov_f = 1.f / std::tan(fov * 0.5f * static_cast<float>(M_PI) / 180.f);
        const float z_normalization_f = z_far / (z_far - z_near);

        // clang-format off
        return matrix4x4f {
            { fov_f / aspect_ratio,     0,                            0, 0 },
            {                    0, fov_f,                            0, 0 },
            {                    0,     0,            z_normalization_f, 1 },
            {                    0,     0,  -z_near * z_normalization_f, 0 }
        };
        // clang-format on
    }

    template <typename T, int rows, int columns>
    const T *as_array(const linalg::mat<T, rows, columns> &m) noexcept
    {
        return reinterpret_cast<const T *>(&m);
    }
} // namespace matrix

#endif /* !MATRIX_HH */
