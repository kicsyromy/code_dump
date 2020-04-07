#include "cartridge.hh"
#include "cpu6502.hh"
#include "ppu2c02.hh"

#include <spdlog/spdlog.h>

#include <memory>

Cartridge::Cartridge(Cpu6502 *cpu, Ppu2C02 *ppu) noexcept
  : Device{ &read_data, &write_data }
  , cpu_{ *cpu }
  , ppu_{ *ppu }
{}

bool Cartridge::load(std::string_view path) noexcept
{
    /* clang-format off */
    std::unique_ptr<FILE, decltype(&std::fclose)> rf{
        std::fopen(path.data(), "rb"),
        &std::fclose
    };
    auto rom_file = rf.get();
    /* clang-format on */

    if (rom_file == nullptr)
    {
        const auto err = errno;
        spdlog::error("Failed to load rom from {}: {}", path, strerror(err));
        return false;
    }

    std::fread(header_.name, 4, 1, rom_file);
    std::fread(&header_.program_rom_size, 1, 1, rom_file);
    std::fread(&header_.character_rom_size, 1, 1, rom_file);
    std::fread(&header_.mapper1, 1, 1, rom_file);
    std::fread(&header_.mapper2, 1, 1, rom_file);
    std::fread(&header_.program_ram_size, 1, 1, rom_file);
    std::fread(&header_.tv_system1, 1, 1, rom_file);
    std::fread(&header_.tv_system2, 1, 1, rom_file);
    std::fread(header_.unused, 5, 1, rom_file);

    // 512-byte trainer at $7000-$71FF
    if (header_.mapper1 & 0x04) std::fseek(rom_file, 512, SEEK_CUR);

    mapper_id_ = u8(((header_.mapper2 >> 4) << 4) | (header_.mapper1 >> 4));

    std::uint8_t file_format{ 1 };
    if (file_format == 0) {}
    if (file_format == 1)
    {
        program_banks_ = header_.program_rom_size;
        program_memory_.resize(program_banks_ * 16 * 1024);
        std::fread(program_memory_.data(), program_memory_.size(), 1, rom_file);

        character_banks_ = header_.character_rom_size;
        character_memory_.resize(character_banks_ * 8 * 1024);
        std::fread(character_memory_.data(), character_memory_.size(), 1, rom_file);
    }
    if (file_format == 2) {}

    return true;
}

std::pair<bool, std::uint8_t> Cartridge::read_data(std::uint16_t address,
    const void *instance) noexcept
{
    auto self = static_cast<const Ppu2C02 *>(instance);
    //    return { true, self->RAM[address & 0x0007] };
    static_cast<void>(address);
    static_cast<void>(self);
    return { false, 0 };
}

bool Cartridge::write_data(std::uint16_t address, std::uint8_t value, void *instance) noexcept
{
    auto self = static_cast<Ppu2C02 *>(instance);
    //    self->RAM[address & 0x0007] = value;
    static_cast<void>(address);
    static_cast<void>(value);
    static_cast<void>(self);
    return false;
}
