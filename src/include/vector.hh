#ifndef VECTOR_HH
#define VECTOR_HH

#include <cmath>

#include <array>

#include <linalg/linalg.h>

using vector3f = linalg::vec<float, 3>;
using vector4f = linalg::vec<float, 4>;

template <typename value_t>
constexpr linalg::vec<value_t, 3> operator-(const linalg::vec<value_t, 3> &v1,
                                            const linalg::vec<value_t, 3> &v2) noexcept
{
    return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
}

template <typename value_t>
constexpr linalg::vec<value_t, 4> operator-(const linalg::vec<value_t, 4> &v1,
                                            const linalg::vec<value_t, 4> &v2) noexcept
{
    return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w };
}

#endif /* !VECTOR_HH */
