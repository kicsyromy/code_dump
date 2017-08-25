#include <iostream>
#include <cmath>

#include <dpmi.h>
#include <sys/nearptr.h>
#include <dos.h>
#include <conio.h>

#include "logger.h"
#include "color.h"

constexpr const unsigned short VIDEO_TEXT_MODE     { 0x03 };
constexpr const unsigned short VIDEO_GRAPHICS_MODE { 0x13 };
constexpr const int BIOS_VIDEO_FUNCTIONS_INTERRUPT { 0x10 };

constexpr const int SCREEN_WIDTH  { 320 };
constexpr const int SCREEN_HEIGHT { 200 };

void start_graphics_mode()
{
    __dpmi_regs regs { 0 };
    regs.x.ax = VIDEO_GRAPHICS_MODE;
    __dpmi_int(BIOS_VIDEO_FUNCTIONS_INTERRUPT, &regs);
}

void start_text_mode()
{
    __dpmi_regs regs { 0 };
    regs.x.ax = VIDEO_TEXT_MODE;
    __dpmi_int(BIOS_VIDEO_FUNCTIONS_INTERRUPT, &regs);
}

void put_pixel(int x, int y, std::int8_t color)
{
    static constexpr const auto multiply_by_320 = [](int value) { return (value << 8) + (value << 6); };
    /* http://www.delorie.com/djgpp/doc/brennan/brennan_access_vga.html */
    static std::uint8_t *vga_memory_buffer { reinterpret_cast<std::uint8_t *>(0xa0000 + __djgpp_conventional_base) };
    vga_memory_buffer[multiply_by_320(y) + x] = color;
}

int main()
{
    /* Disable DOS memory protection in order to be able to access the */
    /* the video memory directly                                       */
    if (__djgpp_nearptr_enable() == 0)
    {
        LOG_FATAL("Failed to disable DOS memory protection, needed for graphics. Press any key to exit.");
        getch();
    }
    LOG_INFO("Press any key to enter the wonderfull world of graphics...");
    LOG_INFO("Size of pointer: %x, size of int %x", sizeof(std::intptr_t), sizeof(int));
    LOG_INFO("Color count: %d", color::from_rgb<44, 48, 85>() + 1);
    getch();
    start_graphics_mode();

    put_pixel(1, 1, color::from_rgb<44, 48, 85>());
    put_pixel(1, 2, color::from_rgb<44, 48, 85>());
    put_pixel(1, 3, color::from_rgb<44, 48, 85>());
    put_pixel(1, 4, color::from_rgb<44, 48, 85>());
    put_pixel(1, 5, color::from_rgb<44, 48, 85>());
    put_pixel(2, 1, color::from_rgb<44, 48, 85>());
    put_pixel(2, 2, color::from_rgb<44, 48, 85>());
    put_pixel(2, 3, color::from_rgb<44, 48, 85>());
    put_pixel(2, 4, color::from_rgb<44, 48, 85>());
    put_pixel(2, 5, color::from_rgb<44, 48, 85>());

    getch();
    start_text_mode();

    return 0;
}
