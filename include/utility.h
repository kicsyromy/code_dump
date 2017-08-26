#ifndef UTILITY_H
#define UTILITY_H

#include <cstdint>
#include <type_traits>

using byte_t = std::uint8_t *;
using word_t = std::uint16_t *;

namespace utility
{
    template <typename T>
    constexpr std::enable_if_t<std::is_integral_v<T>, byte_t> memory_at(T &&v)
    {
        return reinterpret_cast<byte_t>(v);
    }
}

#endif // UTILITY_H
