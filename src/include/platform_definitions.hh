#pragma once

#include <cstdint>

struct AddressRange
{
    std::uint16_t lower_bound;
    std::uint16_t upper_bound;
};

/* CPU Bus Addresses */
constexpr AddressRange CART_CPU_BUS_ADDRESS{ 0x4020, 0xFFFF };
constexpr AddressRange RAM_CPU_BUS_ADDRESS{ 0x0000, 0x1FFF };
constexpr AddressRange PPU_CPU_BUS_ADDRESS{ 0x2000, 0x3FFF };

/* PPU Bus Addresses */
constexpr AddressRange CART_PPU_BUS_ADDRESS{ 0x0000, 0x1FFF };
constexpr AddressRange PATTERN_TABLE_PPU_BUS_ADDRESS{ CART_PPU_BUS_ADDRESS };
constexpr AddressRange NAME_TABLE_PPU_BUS_ADDRESS{ 0x2F00, 0x3EFF };
constexpr AddressRange PALETTE_TABLE_PPU_BUS_ADDRESS{ 0x3F00, 0x3FFF };

/* Misc definistion */
constexpr std::size_t BYTES_PER_PIXEL{ 4 };
