#ifndef VECTOR_HH
#define VECTOR_HH

#include <cmath>

#include <array>

template <typename value_t, std::size_t count> struct vec
{
    using element_t = value_t;
    std::array<value_t, count> elements;
};

struct vector3f : public vec<float, 3>
{
    constexpr float &x() noexcept { return elements[0]; }
    constexpr const float &x() const noexcept { return elements[0]; }

    constexpr float &y() noexcept { return elements[1]; }
    constexpr const float &y() const noexcept { return elements[1]; }

    constexpr float &z() noexcept { return elements[2]; }
    constexpr const float &z() const noexcept { return elements[2]; }

    constexpr void normalize() noexcept
    {
        /* Pythagorean Theorem */
        const auto l = std::sqrt(x() * x() + y() * y() + z() * z());

        x() /= l;
        y() /= l;
        z() /= l;
    }

    constexpr vector3f normalized() const noexcept
    {
        auto result = *this;
        result.normalize();
        return result;
    }

    constexpr float dot_product(const vector3f &other) const noexcept
    {
        return x() * other.x() + y() * other.y() + z() * other.z();
    }
};

struct vector4f : public vec<float, 4>
{
    constexpr float &x() noexcept { return elements[0]; }
    constexpr const float &x() const noexcept { return elements[0]; }

    constexpr float &y() noexcept { return elements[1]; }
    constexpr const float &y() const noexcept { return elements[1]; }

    constexpr float &z() noexcept { return elements[2]; }
    constexpr const float &z() const noexcept { return elements[2]; }

    constexpr float &w() noexcept { return elements[3]; }
    constexpr const float &w() const noexcept { return elements[3]; }
};

#endif /* !VECTOR_HH */
