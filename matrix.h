#ifndef ENGINE_MATRIX_H
#define ENGINE_MATRIX_H

#include <array>

namespace engine
{
    template<std::size_t rowCount, std::size_t columnCount = rowCount>
    class matrix
    {
        using this_t = matrix<rowCount, columnCount>;

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
            return std::move(result);
        }

    private:
        std::array<double, rowCount * columnCount> data_;
    };
}

#endif // ENGINE_MATRIX_H
