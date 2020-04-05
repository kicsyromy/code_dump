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

        set_value(static_cast<ValueType>(value));
    }

    constexpr ValueType &get() noexcept
    {
        return *static_cast<ValueType *>(static_cast<void *>(data_.data()));
    }

    constexpr const ValueType &get() const noexcept
    {
        return *static_cast<const ValueType *>(static_cast<const void *>(data_.data()));
    }

    constexpr void set(ValueType value) noexcept { get() = value; }

    template<typename FlagType> constexpr bool is_flag_set(FlagType flag) const noexcept
    {
        static_assert(sizeof(FlagType) <= sizeof(ValueType));
        static_assert(std::is_convertible_v<FlagType, std::uint64_t>);

        const auto v = get();
        return static_cast<bool>(v & flag);
    }

    template<typename IndexType> constexpr bool is_bit_set(IndexType bit_index) const noexcept
    {
        static_assert(std::is_integral_v<IndexType>);

        return is_flag_set(1 << bit_index);
    }

    template<typename FlagType> constexpr void set_flag(FlagType flag, bool value) noexcept
    {
        static_assert(sizeof(FlagType) <= sizeof(ValueType));
        static_assert(std::is_convertible_v<FlagType, std::uint64_t>);

        const auto v = get();

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

    constexpr std::uint8_t &high_byte() noexcept
    {
        if constexpr (sizeof(ValueType) == 2) { return data_[1]; }
        else
        {
            return data_[0];
        }
    }

    constexpr const std::uint8_t &high_byte() const noexcept
    {
        if constexpr (sizeof(ValueType) == 2) { return data_[1]; }
        else
        {
            return data_[0];
        }
    }

    constexpr std::uint8_t &low_byte() noexcept { return data_[0]; }

    constexpr const std::uint8_t &low_byte() const noexcept { return data_[0]; }

private:
    constexpr ValueType get_value() noexcept
    {
        ValueType result{ 0 };
        for (int i = static_cast<int>(sizeof(ValueType)) - 1; i >= 0; --i)
        {
            result = static_cast<ValueType>(result << 8);
            result = static_cast<ValueType>(result | data_[static_cast<std::size_t>(i)]);
        }

        return result;
    }

    constexpr void set_value(ValueType value) noexcept
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
