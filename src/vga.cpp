#include "vga.h"

#include <dpmi.h>
#include <sys/nearptr.h>

namespace
{
    constexpr const unsigned short VIDEO_TEXT_MODE     { 0x03 };
    constexpr const unsigned short VIDEO_GRAPHICS_MODE { 0x13 };
    constexpr const int BIOS_VIDEO_FUNCTIONS_INTERRUPT { 0x10 };

    constexpr const int VIDEO_MEMORY_ADDRESS_OFFSET { 0xa0000 };
}

/* http://www.delorie.com/djgpp/doc/brennan/brennan_access_vga.html */
VGA::VGA()
  : vga_memory_buffer_ {
        reinterpret_cast<std::uint8_t *>(VIDEO_MEMORY_ADDRESS_OFFSET + __djgpp_conventional_base)
    }
{
    __dpmi_regs regs { 0 };
    regs.x.ax = VIDEO_GRAPHICS_MODE;
    __dpmi_int(BIOS_VIDEO_FUNCTIONS_INTERRUPT, &regs);
}

VGA::~VGA()
{
    __dpmi_regs regs { 0 };
    regs.x.ax = VIDEO_TEXT_MODE;
    __dpmi_int(BIOS_VIDEO_FUNCTIONS_INTERRUPT, &regs);
}
