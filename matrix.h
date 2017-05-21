#ifndef ENGINE_MATRIX_H
#define ENGINE_MATRIX_H

#include <array>
#include <initializer_list>
#include <cassert>

int f();

namespace engine
{
    template<std::size_t rowCount, std::size_t columnCount = rowCount>
    class matrix
    {
        using this_t = matrix<rowCount, columnCount>;

    public:
        constexpr matrix()
            : data_()
        {}

        matrix(std::initializer_list<std::initializer_list<double>> data)
            : matrix()
        {
            assert(data.size() == rowCount);

            std::size_t pos = 0;
            for (auto it = data.begin(); it != data.end(); ++it)
            {
                const auto &row = *it;
                assert(row.size() == columnCount);
                for (auto jt = row.begin(); jt != row.end(); ++jt)
                {
                    data_[pos++] = *jt;
                }
            }
        }

    public:

    public:
        this_t &&operator +(const this_t &other)
        {
            this_t result;
            for (std::size_t it = 0; it < data_.size(); ++it)
            {
                result.data_[it] = data_[it] + other.data_[it];
            }

            return std::move(result);
        }

        this_t &&operator *(const double scalar) const
        {
            this_t result;
            for (std::size_t it = 0; it < data_.size(); ++it)
            {
                result[it] = data_[it] * scalar;
            }

            return std::move(result);
        }

        this_t &operator *=(const double scalar)
        {
            for (std::size_t it = 0; it < data_.size(); ++it)
            {
                data_[it] *= scalar;
            }

            return *this;
        }

        this_t &&operator /(const double scalar) const
        {
            this_t result;
            for (std::size_t it = 0; it < data_.size(); ++it)
            {
                result[it] = data_[it] / scalar;
            }

            return std::move(result);
        }

        this_t &operator /=(const double scalar)
        {
            for (std::size_t it = 0; it < data_.size(); ++it)
            {
                data_[it] /= scalar;
            }

            return *this;
        }

        template<std::size_t otherColumnCount>
        matrix<rowCount, otherColumnCount> &&operator *(const matrix<columnCount, otherColumnCount> &other) const
        {
            matrix<rowCount, otherColumnCount> result;
            for (std::size_t it = 0; it < rowCount; ++it)
            {
                for (std::size_t jt = 0; jt < otherColumnCount; ++jt)
                {
                    for (std::size_t kt = 0; kt < columnCount; ++kt)
                    {
                        result.data_[it * otherColumnCount + jt] += data_[it * columnCount + kt] * other.data_[kt * otherColumnCount + jt];
                    }
                }
            }

            return std::move(result);
        }

        constexpr double operator ()(int x, int y)
        {
            return data_[x * columnCount + y];
        }

    private:
        std::array<double, rowCount * columnCount> data_;

    private:
        template<std::size_t r, std::size_t c>
        friend class matrix;
        friend struct math;
    };

    using matrix3d = matrix<3, 3>;

    struct math
    {
        template<std::size_t rc, std::size_t cc>
        static matrix<rc, cc> identity()
        {
            static_assert(rc == cc, "only square matrices can have an identity matrix");

            matrix<rc, cc> result;
            for (std::size_t it = 0, column = 0; it < rc; ++it)
            {
                result.data_[column] = 1;
                column += rc + 1;
            }

            return result;
        }

        template<std::size_t rc, std::size_t cc>
        static constexpr matrix<rc - 1, cc -1> mminor(const matrix<rc, cc> &m, std::size_t row, std::size_t column)
        {
            static_assert((rc > 1) && (cc > 1), "the matrix is to small to have a minor");

            matrix<rc - 1, cc -1> result;

            std::size_t index = 0;
            for (std::size_t it = 0; it < rc; ++it)
            {
                for (std::size_t jt = 0; jt < rc; ++jt)
                {
                    if ((it != row) && (jt != column))
                    {
                        result.data_[index] = m.data_[it * cc + jt];
                        ++index;
                    }
                }
            }

            return result;
        }

        template<std::size_t rc, std::size_t cc>
        static constexpr typename std::enable_if<(rc > 2), double>::type determinant(const matrix<rc, cc> &m)
        {
            static_assert(rc == cc, "only square matrices can have a determinant");

            double result = 0;
            int sign = 1;

            for (std::size_t it = 0; it < cc; ++it)
            {
                result += sign * (m.data_[it] * determinant(mminor(m, 0, it)));
                sign *= -1;
            }

            return result;
        }

        template<std::size_t rc, std::size_t cc>
        static constexpr typename std::enable_if<(rc == 2), double>::type determinant(const matrix<rc, cc> &m)
        {
            static_assert(rc == cc, "only square matrices can have a determinant");

            return m.data_[0] * m.data_[3] - m.data_[1] * m.data_[2];
        }

        template<std::size_t rc, std::size_t cc>
        static constexpr typename std::enable_if<(rc < 2), double>::type determinant(const matrix<rc, cc> &m)
        {
            static_assert(rc == cc, "only square matrices can have a determinant");
            return 0;
        }
    };
}

#endif // ENGINE_MATRIX_H
