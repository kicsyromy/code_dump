#ifndef SOUND_H
#define SOUND_H

#include <cstdint>
#include <cstdlib>
#include <string_view>

#include <dpmi.h>
#include <dos.h>
#include <conio.h>
#include <unistd.h>

#include "logger.h"
#include "utility.h"

//namespace assembly_example
//{
//    __asm__ __volatile__ (
//        "mov $0x86, %%ah\n"
//        "mov $10, %%cx\n"
//        "mov $0, %%dx\n"
//        "int $0x15"
//        : : : "ax", "dx", "cx"
//    );
//}

struct SoundBlaster
{
    void test()
    {
        LOG_INFO("BLASTER = %s", getenv("BLASTER"));
        LOG_INFO("Reset port: %x\nRead port: %x\nWrite port: %x\nPoll port: %x\ni16bit_port: %x\n"
                 "IRQ: %x\nDMA: %x\n", ports_.reset_port(), ports_.read_port(), ports_.write_port(),
                 ports_.poll_port(), ports_.i16bit_port(), irq_, dma_channel_);
    }

    inline SoundBlaster()
      : env_blaster_(getenv("BLASTER"))
      , irq_(get_irq())
      , dma_channel_(get_dma_channel())
      , ports_(env_blaster_)
      , dsp_(ports_)
    {
        dsp_.reset();
    }

private:
    inline std::uint8_t get_irq()
    {
        std::uint8_t irq = 0;
        const auto env_blaster_length = env_blaster_.size();

        const auto irq_start_pos = env_blaster_.find_first_of('I');
        if (env_blaster_length && (irq_start_pos < env_blaster_length - 1))
        {
            const char *irq_str = &env_blaster_[irq_start_pos + 1];
            irq = strtoul(irq_str, nullptr, 16);
        }

        return irq;
    }

    inline std::uint8_t get_dma_channel()
    {
        std::uint8_t dma_channel = 0;
        const auto env_blaster_length = env_blaster_.size();

        const auto dma_channel_start_pos = env_blaster_.find_first_of('D');
        if (env_blaster_length && (dma_channel_start_pos < env_blaster_length - 1))
        {
            const char *dma_channel_str = &env_blaster_[dma_channel_start_pos + 1];
            dma_channel = strtoul(dma_channel_str, nullptr, 16);
        }

        return dma_channel;
    }

    class Ports
    {
    public:
        Ports(const std::string_view &env_blaster)
          : io_base_address_(get_io_base_address(env_blaster))
          , reset_port_(get_reset_port())
          , read_port_(get_read_port())
          , write_port_(get_write_port())
          , poll_port_(get_poll_port())
          , i16bit_port_(get_i16bit_port())
        {
        }
        ~Ports() = default;

    public:
        inline std::uint16_t reset_port()  const { return reset_port_;  }
        inline std::uint16_t read_port()   const { return read_port_;   }
        inline std::uint16_t write_port()  const { return write_port_;  }
        inline std::uint16_t poll_port()   const { return poll_port_;   }
        inline std::uint16_t i16bit_port() const { return i16bit_port_; }

    private:
        std::uint16_t get_io_base_address(const std::string_view &env_blaster)
        {
            std::uint16_t io_address = 0x220;
            const auto env_blaster_length = env_blaster.size();

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

            return io_address;
        }

        inline std::uint16_t get_reset_port()  { return (io_base_address_ + 0x6); }
        inline std::uint16_t get_read_port()   { return (io_base_address_ + 0xa); }
        inline std::uint16_t get_write_port()  { return (io_base_address_ + 0xc); }
        inline std::uint16_t get_poll_port()   { return (io_base_address_ + 0xe); }
        inline std::uint16_t get_i16bit_port() { return (io_base_address_ + 0xf); }

    private:
        std::uint16_t io_base_address_;
        std::uint16_t reset_port_;
        std::uint16_t read_port_;
        std::uint16_t write_port_;
        std::uint16_t poll_port_;
        std::uint16_t i16bit_port_;
    };

    class DSP
    {
    public:
        DSP(const Ports &ports)
          : ports_(ports)
        {}
        ~DSP() = default;

    public:
        inline void reset()
        {
            usleep(1000);
        }

    private:
        inline byte_t read()
        {
            __asm__ __volatile__(
                "mov %0,%%dx\n"
                "read_value:\n"
                "in %%dx, %%al\n"
                "and 128,%%al\n"
                "jz read_value\n"
                "mov %1,%%dx\n"
                "in %%dx,%%al\n"
                "ret\n"
                : : "r"(ports_.poll_port()), "r"(ports_.read_port())
                : "ax", "dx"
            );
        }

        inline void write(std::uint16_t port, byte_t value)
        {
            __asm__ __volatile__(
                "mov %0,%%dx\n"
                "read_value:\n"
                "in %%dx, %%al\n"
                "and 128,%%al\n"
                "jnz read_value\n"
                "mov %1,%%al\n"
                "out %%al,%%dx\n"
                : : "r"(port), "r"(value)
                : "ax", "dx"
            );
        }
    private:
        const Ports &ports_;
    };

private:
    const std::string_view env_blaster_;
    const std::uint8_t irq_;
    const std::uint8_t dma_channel_;
    const Ports ports_;
    DSP dsp_;
};

#endif // SOUND_H
