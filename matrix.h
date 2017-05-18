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

        this_t &&operator +(const this_t &other)
        {
            this_t result;
            for (std::size_t it = 0; it < data_.size(); ++it)
            {
                result.data_[it] = data_[it] + other.data_[it];
            }

            return std::move(result);
        }

        this_t &&operator *(const double scalar)
        {
            this_t result;
            for (std::size_t it = 0; it < data_.size(); ++it)
            {
                result[it] = scalar * data_[it];
            }

            return std::move(result);
        }

        template<std::size_t otherColumnCount>
        matrix<rowCount, otherColumnCount> &&operator *(const matrix<columnCount, otherColumnCount> &other)
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

        double operator ()(int x, int y)
        {
            return x * columnCount + y;
        }

    private:
        std::array<double, rowCount * columnCount> data_;

    private:
        template<std::size_t r, std::size_t c>
        friend class matrix;
    };
}

#endif // ENGINE_MATRIX_H
