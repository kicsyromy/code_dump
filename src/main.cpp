#include <sys/nearptr.h>
#include <conio.h>

#include "logger.h"
#include "vga.h"

#include <iostream>

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

    getch();

    VGA vga;
    vga.put_pixel(1, 1, Color<15, 68, 51>());
    vga.put_pixel(1, 2, Color<15, 68, 51>());
    vga.put_pixel(1, 3, Color<15, 68, 51>());
    vga.put_pixel(1, 4, Color<15, 68, 51>());
    vga.put_pixel(1, 5, Color<15, 68, 51>());
    vga.put_pixel(2, 1, Color<15, 68, 51>());
    vga.put_pixel(2, 2, Color<15, 68, 51>());
    vga.put_pixel(2, 3, Color<15, 68, 51>());
    vga.put_pixel(2, 4, Color<15, 68, 51>());
    vga.put_pixel(2, 5, Color<15, 68, 51>());

    getch();

    return 0;
}
