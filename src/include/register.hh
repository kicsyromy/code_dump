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

    constexpr ValueType value() const noexcept
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

    template<typename FlagType> constexpr bool is_flag_set(FlagType flag) const noexcept
    {
        static_assert(std::is_integral_v<FlagType>);

        const auto v = value();
        return static_cast<bool>(v & flag);
    }

    template<typename IndexType> constexpr bool is_bit_set(IndexType bit_index) const noexcept
    {
        static_assert(std::is_integral_v<IndexType>);

        return is_flag_set(1 << bit_index);
    }

    template<typename FlagType> constexpr void set_flag(FlagType flag, bool value) noexcept
    {
        static_assert(std::is_integral_v<FlagType>);

        const auto v = Register::value();

        if (value)
            set(static_cast<ValueType>(v | flag));
        else
            set(static_cast<ValueType>(v & ~flag));
    }

    template<typename IndexType> constexpr void set_bit(IndexType bit_index, bool value) noexcept
    {
        static_assert(std::is_integral_v<IndexType>);
        set_flag(1 << bit_index, value);
    }

private:
    std::array<std::uint8_t, sizeof(ValueType)> data_{};
};

using Register8Bit = Register<std::uint8_t>;
using Register16Bit = Register<std::uint16_t>;
