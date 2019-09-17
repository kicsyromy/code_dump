#ifndef VGA_HH
#define VGA_HH

#include <cstdint>
#include <cstring>

#include <dpmi.h>
#include <go32.h>
#include <sys/movedata.h>
#include <sys/nearptr.h>

#include "color.hh"
#include "utility.hh"

class VGA
{
public:
    static constexpr const std::size_t SCREEN_WIDTH{ 320 };
    static constexpr const std::size_t SCREEN_HEIGHT{ 200 };

private:
    static constexpr const unsigned short VIDEO_TEXT_MODE{ 0x03 };
    static constexpr const unsigned short VIDEO_GRAPHICS_MODE{ 0x13 };
    static constexpr const int BIOS_VIDEO_FUNCTIONS_INTERRUPT{ 0x10 };

    static constexpr const int VIDEO_MEMORY_ADDRESS_OFFSET{ 0xa0000 };

public:
    inline VGA()
      /* http://www.delorie.com/djgpp/doc/brennan/brennan_access_vga.html */
      : vga_memory_buffer_{ utility::memory_at(VIDEO_MEMORY_ADDRESS_OFFSET +
                                               __djgpp_conventional_base) }
    {
        __dpmi_regs registers;
        std::memset(&registers, 0, sizeof(__dpmi_regs));
        registers.x.ax = VIDEO_GRAPHICS_MODE;
        __dpmi_int(BIOS_VIDEO_FUNCTIONS_INTERRUPT, &registers);
    }

    inline ~VGA()
    {
        __dpmi_regs registers;
        std::memset(&registers, 0, sizeof(__dpmi_regs));
        registers.x.ax = VIDEO_TEXT_MODE;
        __dpmi_int(BIOS_VIDEO_FUNCTIONS_INTERRUPT, &registers);
    }

private:
    static constexpr auto multiply_by_320(int value) { return (value << 8) + (value << 6); }

public:
    template <std::uint8_t r, std::uint8_t g, std::uint8_t b>
    constexpr void put_pixel(int x, int y, const Color<r, g, b> &c)
    {
        vga_memory_buffer_[multiply_by_320(y) + x] = c.code();
    }

    template <std::size_t size> inline void output(const std::uint32_t *rgba_pixel_data) noexcept
    {
        //        std::array<std::uint8_t, size> transformed_pixel_data;
        //        for (auto pixel = 0ul; pixel < size; ++pixel)
        //        {
        //            transformed_pixel_data[pixel] = color_map_.from_32bit(rgba_pixel_data[pixel]);
        //        }

        //        std::memcpy(vga_memory_buffer_, transformed_pixel_data.data(),
        //                    transformed_pixel_data.size());
    }

private:
    byte_t *vga_memory_buffer_;

private:
    color_map color_map_;

public:
    VGA(const VGA &) = delete;
    VGA(VGA &&) = default;
    VGA &operator=(const VGA &) = delete;
    VGA &operator=(VGA &&) = default;
};

#endif /* !VGA_HH */
