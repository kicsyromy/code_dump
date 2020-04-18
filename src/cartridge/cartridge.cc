#include "cartridge.hh"
#include "cpu6502.hh"
#include "mapper_000.hh"
#include "pattern_table.hh"
#include "platform_definitions.hh"
#include "ppu2c02.hh"

#include <spdlog/spdlog.h>

#include <memory>

Cartridge::Cartridge(Cpu6502 *cpu, Ppu2C02 *ppu) noexcept
  : Device{ &read_request, &write_request, "Cartridge" }
  , cpu_{ *cpu }
  , ppu_{ *ppu }
  , pattern_table_{ *this }
{}

bool Cartridge::load(std::string_view path) noexcept
{
    std::unique_ptr<FILE, decltype(&std::fclose)> rf{ std::fopen(path.data(), "rb"), &std::fclose };
    auto rom_file = rf.get();

    if (rom_file == nullptr)
    {
        const auto err = errno;
        spdlog::error("Failed to load rom from {}: {}", path, strerror(err));
        return false;
    }

    auto res = std::fread(header_.name, 4, 1, rom_file);
    res = std::fread(&header_.program_rom_size, 1, 1, rom_file);
    res = std::fread(&header_.character_rom_size, 1, 1, rom_file);
    res = std::fread(&header_.mapper1, 1, 1, rom_file);
    res = std::fread(&header_.mapper2, 1, 1, rom_file);
    res = std::fread(&header_.program_ram_size, 1, 1, rom_file);
    res = std::fread(&header_.tv_system1, 1, 1, rom_file);
    res = std::fread(&header_.tv_system2, 1, 1, rom_file);
    res = std::fread(header_.unused, 5, 1, rom_file);

    // 512-byte trainer at $7000-$71FF
    if (header_.mapper1 & 0x04) std::fseek(rom_file, 512, SEEK_CUR);

    mapper_id_ = u8(((header_.mapper2 >> 4) << 4) | (header_.mapper1 >> 4));

    std::uint8_t file_format{ 1 };
    if (file_format == 0) {}
    if (file_format == 1)
    {
        program_banks_ = header_.program_rom_size;
        program_memory_.resize(program_banks_ * 16ul * 1024ul);
        res = std::fread(program_memory_.data(), program_memory_.size(), 1, rom_file);

        character_banks_ = header_.character_rom_size;
        character_memory_.resize(character_banks_ * 8ul * 1024ul);
        res = std::fread(character_memory_.data(), character_memory_.size(), 1, rom_file);

        mapper_cpu_.read_f = &Mapper000::cpu_read;
        mapper_cpu_.write_f = &Mapper000::cpu_write;
        mapper_ppu_.read_f = &Mapper000::ppu_read;
        mapper_ppu_.write_f = &Mapper000::ppu_write;
    }
    if (file_format == 2) {}

    return true;

    static_cast<void>(res);
}

#define get_mapper(address)                                 \
    if (address >= CART_CPU_BUS_ADDRESS.lower_bound &&      \
        address <= CART_CPU_BUS_ADDRESS.upper_bound)        \
    { return { &mapper_cpu_, &cpu_, &program_memory_ }; }   \
                                                            \
    if (address >= CART_PPU_BUS_ADDRESS.lower_bound &&      \
        address <= CART_PPU_BUS_ADDRESS.upper_bound)        \
    { return { &mapper_ppu_, &ppu_, &character_memory_ }; } \
                                                            \
    return { nullptr, nullptr, nullptr };

std::tuple<Cartridge::MapperInterface *, void *, std::vector<std::uint8_t> *> Cartridge::
    get_mapper_interface(uint16_t address) noexcept
{
    get_mapper(address);
}

std::tuple<const Cartridge::MapperInterface *, const void *, const std::vector<uint8_t> *>
Cartridge::get_mapper_interface(uint16_t address) const noexcept
{
    get_mapper(address);
}

std::pair<bool, std::uint8_t> Cartridge::read_request(std::uint16_t address,
    const void *instance) noexcept
{
    auto self = static_cast<const Cartridge *>(instance);

    auto [mapper_interface, device, memory] = self->get_mapper_interface(address);

    if (mapper_interface == nullptr || mapper_interface->read_f == nullptr)
        return { false, std::uint8_t(0) }; 

    const auto mapped_address = mapper_interface->read_f(address, *self, device);
    if (mapped_address.first == false) { return { false, std::uint8_t(0) }; }

    return { true, (*memory)[mapped_address.second] };
}

bool Cartridge::write_request(std::uint16_t address, std::uint8_t value, void *instance) noexcept
{
    auto self = static_cast<Cartridge *>(instance);

    auto [mapper_interface, device, memory] = self->get_mapper_interface(address);

    if (mapper_interface == nullptr || mapper_interface->write_f == nullptr) { return false; }

    const auto mapped_address = mapper_interface->write_f(address, *self, device);
    if (mapped_address.first == false) { return false; }

    (*memory)[mapped_address.second] = value;
    return true;
}
