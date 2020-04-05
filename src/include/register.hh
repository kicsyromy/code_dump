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

private:
    std::array<std::uint8_t, sizeof(ValueType)> data_{};
};

using Register8Bit = Register<std::uint8_t>;
using Register16Bit = Register<std::uint16_t>;
