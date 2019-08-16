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
