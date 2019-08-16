#ifndef VECTOR_HH
#define VECTOR_HH

#include <array>

#include <linalg/linalg.h>

struct vector3f
{
    enum axis
    {
        X,
        Y,
        Z,
        W
    };

    std::array<float, 4> coordinates;

    constexpr vector3f(const float x, const float y, const float z, const float w = 1.f) noexcept
      : coordinates{ x, y, z, w }
    {
    }

    constexpr vector3f(const linalg::vec<float, 3> &v) noexcept : coordinates{ v.x, v.y, v.z, 1.f }
    {
    }

    constexpr vector3f(const linalg::vec<float, 4> &v) noexcept : coordinates{ v.x, v.y, v.z, v.w }
    {
    }

    constexpr float &x() noexcept { return coordinates[axis::X]; }
    constexpr float x() const noexcept { return coordinates[axis::X]; }
    constexpr float &y() noexcept { return coordinates[axis::Y]; }
    constexpr float y() const noexcept { return coordinates[axis::Y]; }
    constexpr float &z() noexcept { return coordinates[axis::Z]; }
    constexpr float z() const noexcept { return coordinates[axis::Z]; }
    constexpr float &w() noexcept { return coordinates[axis::W]; }
    constexpr float w() const noexcept { return coordinates[axis::W]; }

    constexpr linalg::vec<float, 3> as_v3f() const noexcept
    {
        return { coordinates[0], coordinates[1], coordinates[2] };
    }

    constexpr linalg::vec<float, 4> as_v4f() const noexcept
    {
        return { coordinates[0], coordinates[1], coordinates[2], coordinates[3] };
    }
};

#endif /* !VECTOR_HH */
