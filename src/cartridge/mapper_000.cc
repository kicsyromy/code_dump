#include "mapper_000.hh"

std::pair<bool, std::uint16_t> Mapper000::cpu_read(std::uint16_t input_address,
    Cartridge &cart,
    const void *cpu) noexcept
{
    static_cast<void>(input_address);
    static_cast<void>(cart);
    static_cast<void>(cpu);

    return {};
}

std::pair<bool, std::uint16_t> Mapper000::cpu_write(std::uint16_t output_address,
    Cartridge &cart,
    void *cpu) noexcept
{
    static_cast<void>(output_address);
    static_cast<void>(cart);
    static_cast<void>(cpu);

    return {};
}

std::pair<bool, std::uint16_t> Mapper000::ppu_read(std::uint16_t input_address,
    Cartridge &cart,
    const void *ppu) noexcept
{
    static_cast<void>(input_address);
    static_cast<void>(cart);
    static_cast<void>(ppu);

    return {};
}

std::pair<bool, std::uint16_t> Mapper000::ppu_write(std::uint16_t output_address,
    Cartridge &cart,
    void *ppu) noexcept
{
    static_cast<void>(output_address);
    static_cast<void>(cart);
    static_cast<void>(ppu);

    return {};
}
