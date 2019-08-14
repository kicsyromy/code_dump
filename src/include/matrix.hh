#ifndef MATRIX_HH
#define MATRIX_HH

#include <cstring>

#include <array>
#include <type_traits>

template <std::size_t rows, std::size_t columns, typename value_t = float> struct matrix
{
    using this_t = matrix<rows, columns, value_t>;
    static constexpr auto array_size_{ rows * columns };

    std::array<value_t, array_size_> data_;

    constexpr this_t operator+(const this_t &other) const noexcept
    {
        this_t result;

        for (auto i = 0ull; i < array_size_; ++i)
        {
            result.data_[i] = data_[i] = other.data_[i];
        }

        return result;
    }

    constexpr this_t &operator+=(const this_t &other) noexcept
    {
        for (auto i = 0ull; i < array_size_; ++i)
        {
            data_[i] += other.data_[i];
        }

        return *this;
    }

    template <typename scalar_t> constexpr this_t operator*(scalar_t number)
    {
        static_assert(std::is_scalar_v<scalar_t>, "number must be a scalar value");

        this_t result;
        for (auto i = 0ull; i < array_size_; ++i)
        {
            result.data_[i] = data_[i] * number;
        }

        return result;
    }

    template <typename scalar_t> constexpr this_t &operator*=(scalar_t number)
    {
        static_assert(std::is_scalar_v<scalar_t>, "number must be a scalar value");

        for (auto i = 0ull; i < array_size_; ++i)
        {
            data_[i] *= number;
        }

        return *this;
    }

    template <std::size_t other_columns>
    constexpr matrix<rows, other_columns> operator*(
        const matrix<columns, other_columns, value_t> &other) const noexcept
    {
        matrix<rows, other_columns, value_t> result;

        for (auto result_column = 0ull; result_column < other_columns; ++result_column)
        {
            for (auto i = 0ull; i < rows; ++i)
            {
                value_t row_result = 0;
                const auto local_offset = i * rows;

                for (auto j = 0ull; j < columns; ++j)
                {
                    row_result += data_[local_offset + j] * other.data_[j * rows + result_column];
                }

                result.data_[local_offset + result_column] = row_result;
            }
        }

        return result;
    }

    template <std::size_t other_columns>
    constexpr matrix<rows, other_columns> operator*=(
        const matrix<columns, other_columns, value_t> &other) noexcept
    {
        data_ = this->operator*(other);
    }

    constexpr std::array<value_t, rows> operator*(const std::array<value_t, rows> &vector) const
        noexcept
    {
        std::array<value_t, rows> result;

        for (auto i = 0ull; i < rows; ++i)
        {
            const auto local_offset = i * rows;

            result[i] = 0;
            for (auto j = 0ull; j < columns; ++j)
            {
                result[i] += data_[local_offset + j] * vector[j];
            }
        }

        return result;
    }
};

template <std::size_t rows, std::size_t columns, typename value_t>
inline std::array<value_t, columns> operator*(const std::array<value_t, columns> &vector,
                                              const matrix<rows, columns, value_t> &matrix) noexcept
{
    std::array<value_t, columns> result;

    for (auto i = 0ull; i < rows; ++i)
    {
        const auto local_offset = i * rows;

        result[i] = 0;
        for (auto j = 0ull; j < columns; ++j)
        {
            result[i] += vector[j] * matrix.data_[local_offset + j];
        }
    }

    return result;
}

template <std::size_t size, typename Ts = float>
struct square_matrix : public matrix<size, size, Ts>
{
};

using matrix2x2f = square_matrix<2, float>;
using matrix3x3f = square_matrix<3, float>;
using matrix4x4f = square_matrix<4, float>;

#endif /* !MATRIX_HH */
