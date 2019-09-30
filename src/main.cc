#include <cmath>
#include <cstdio>
#undef PI

#include <array>
#include <chrono>
#include <memory>

#include <allegro.h>

#include <linalg/linalg.h>

#include <string>

#include "input.hh"

static constexpr auto PI = static_cast<float>(M_PI);
static constexpr auto RAD(const float d) noexcept { return PI / 180.f * d; }

int main()
{
    if (allegro_init() != 0)
    {
        std::puts("Failed to start allegro");
        return -1;
    }

    install_timer();
    install_keyboard();
    int buttons = install_mouse();
    if (buttons <= 0)
    {
        allegro_message("Mouse could not be initialized");
    }

    if (install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, nullptr))
    {
        allegro_message("Sound Error: %s", allegro_error);
    }

    set_color_depth(8);
    if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0))
    {
        allegro_message("Video Error: %s", allegro_error);
        return -1;
    }

    textprintf_ex(screen, font, 0, 0, makecol8(255, 0, 0), 0, "Hello, World!");

    for (;;)
    {
        auto [key_value, modifier] = input::read_key();
        if (key_value == KEY_Q)
        {
            std::exit(EXIT_SUCCESS);
        }
    }
}
