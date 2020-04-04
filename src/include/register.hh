#pragma once

#include <array>
#include <cstring>
#include <type_traits>

template<typename ValueType> struct Register
{
    static_assert(std::is_integral_v<ValueType>);
    static_assert(std::is_unsigned_v<ValueType>);

    constexpr Register() noexcept = default;

    template<typename InitialValueType> constexpr Register(InitialValueType value) noexcept
    {
        static_assert(std::is_integral_v<ValueType>);
        static_assert(std::is_convertible_v<InitialValueType, ValueType>);

        set(static_cast<ValueType>(value));
    }

    constexpr ValueType value() noexcept
    {
        ValueType result{ 0 };
        for (int i = static_cast<int>(sizeof(ValueType)) - 1; i >= 0; --i)
        {
            result = static_cast<ValueType>(result << 8);
            result = static_cast<ValueType>(result | data_[static_cast<std::size_t>(i)]);
        }

        return result;
    }

    constexpr void set(ValueType value) noexcept
    {
        for (std::size_t i = 0; i < sizeof(ValueType); ++i)
        {
            data_[i] = static_cast<std::uint8_t>(value & 0xFF);
            value = static_cast<ValueType>(value >> 8);
        }
    }

private:
    std::array<std::uint8_t, sizeof(ValueType)> data_{};
};

using Register8Bit = Register<std::uint8_t>;
using Register16Bit = Register<std::uint16_t>;
