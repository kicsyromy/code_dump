#include "mapper_000.hh"

std::pair<bool, std::uint16_t> Mapper000::cpu_read(std::uint16_t input_address,
    const Cartridge &cart,
    const void *) noexcept
{
    if (input_address >= 0x8000)
    {
        return { true, { uint16_t(input_address & (cart.program_banks() > 1 ? 0x7FFF : 0x3FFF)) } };
    }
    return { false, 0 };
}

std::pair<bool, std::uint16_t> Mapper000::cpu_write(std::uint16_t output_address,
    const Cartridge &cart,
    void *) noexcept
{
    if (output_address >= 0x8000)
    {
        return { true,
            { uint16_t(output_address & (cart.program_banks() > 1 ? 0x7FFF : 0x3FFF)) } };
    }
    return { false, 0 };

    return {};
}

std::pair<bool, std::uint16_t> Mapper000::ppu_read(std::uint16_t input_address,
    const Cartridge &,
    const void *) noexcept
{
    if (input_address <= 0x1FFF) { return { true, { input_address } }; }
    return { false, 0 };
}

std::pair<bool, std::uint16_t> Mapper000::ppu_write(std::uint16_t output_address,
    const Cartridge &cart,
    void *) noexcept
{
    if (output_address <= 0x1FFF)
    {
        if (cart.character_banks() == 0) { return { true, { output_address } }; }
    }
    return { false, 0 };
    return { false, 0 };
}
