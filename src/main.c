#include <allegro.h>

#include <conio.h>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200

extern void print_hello(void);

int main()
{
    allegro_init();

    set_color_depth(8);
    if (set_gfx_mode(GFX_AUTODETECT, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0))
    {
        allegro_message("Video Error: %s", allegro_error);
        getch();
        return -1;
    }

    textout_ex(screen, font, "Hello, World!", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, makecol32(255, 0, 0), 0);
    getch();

    return 0;
}
