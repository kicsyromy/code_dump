#ifndef SOUND_H
#define SOUND_H

#include <cstdint>
#include <cstdlib>
#include <string_view>

#include <conio.h>

#include "logger.h"

struct SoundBlaster
{
    void test()
    {
        LOG_INFO("BLASTER = %s", getenv("BLASTER"));
        LOG_INFO("Reset port: %x\nRead port: %x\nWrite port: %x\nPoll port: %x\ni16bit_port: %x\n"
                 "IRQ: %x\nDMA: %x\n", ports.reset_port_, ports.read_port_, ports.write_port_,
                 ports.poll_port_, ports.i16bit_port_, irq_, dma_channel_);
    }

    inline SoundBlaster()
      : ports()
      , irq_(0)
      , dma_channel_(0)
    {
        const std::string_view env_blaster = getenv("BLASTER");
        const auto env_blaster_length = env_blaster.size();

        std::uint16_t io_address = 0x220;

        const auto io_address_start_pos = env_blaster.find_first_of('A');
        if (env_blaster_length && (io_address_start_pos < env_blaster_length - 1))
        {
            const char *io_address_str = &env_blaster[io_address_start_pos + 1];
            io_address = strtoul(io_address_str, nullptr, 16);
            if (io_address < 0x220 || io_address > 0x260)
            {
                if (io_address > 0)
                {
                    LOG_WARN("The IO base address %x of the Sound Blaster card is out of range."
                             "This could lead to unwanted behavior. You've been warned!", io_address);
                    getch();
                }
                else
                {
                    io_address = 0x220;
                    LOG_WARN("Could not determine a proper value to the IO base address of the Sound"
                             "Blaster card. Defaulting to 0x220");
                    getch();
                }
            }
        }
        else
        {
            LOG_WARN("Could not determine a proper value to the IO base address of the Sound"
                     "Blaster card. Defaulting to 0x220");
            getch();
        }

        LOG_INFO("IO base address: %x", io_address);
        ports.reset_port_  = io_address + 0x6;
        ports.read_port_   = io_address + 0xa;
        ports.write_port_  = io_address + 0xc;
        ports.poll_port_   = io_address + 0xe;
        ports.i16bit_port_ = io_address + 0xf;

        const auto irq_start_pos = env_blaster.find_first_of('I');
        if (env_blaster_length && (irq_start_pos < env_blaster_length - 1))
        {
            const char *irq_str = &env_blaster[irq_start_pos + 1];
            irq_ = strtoul(irq_str, nullptr, 16);
        }

        const auto dma_channel_start_pos = env_blaster.find_first_of('D');
        if (env_blaster_length && (dma_channel_start_pos < env_blaster_length - 1))
        {
            const char *dma_channel_str = &env_blaster[dma_channel_start_pos + 1];
            dma_channel_ = strtoul(dma_channel_str, nullptr, 16);
        }
    }

private:
    struct
    {
        std::uint16_t reset_port_ = 0;
        std::uint16_t read_port_ = 0;
        std::uint16_t write_port_ = 0;
        std::uint16_t poll_port_ = 0;
        std::uint16_t i16bit_port_ = 0; /* I have no clue what this does */
    } ports;
    std::uint8_t irq_;
    std::uint8_t dma_channel_;
};

#endif // SOUND_H
