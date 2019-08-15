#ifndef MATRIX_HH
#define MATRIX_HH

#include <cstring>

#include <array>
#include <type_traits>

#include <linalg/linalg.h>

using matrix2x2f = linalg::mat<float, 2, 2>;
using matrix3x3f = linalg::mat<float, 3, 3>;
using matrix4x4f = linalg::mat<float, 4, 4>;

template <typename value_t, int rows, int columns>
constexpr linalg::vec<value_t, rows> operator*(const linalg::mat<value_t, rows, columns> &m,
                                               const linalg::vec<value_t, columns> &v) noexcept
{
    return linalg::mul(m, v);
}

#endif /* !MATRIX_HH */
