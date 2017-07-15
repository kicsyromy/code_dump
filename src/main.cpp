#include <iostream>

#include <gulp_window.h>
#include <gulp_platform_window_p.h>
#include <gulp_application.h>
#include <gulp_application_p.h>
#include <gulp_layout.h>
#include <gulp_platform_p.h>

int main(int argc, char *argv[])
{
    gulp::Application a(argc, argv);

    gulp::Window w;
    w.show();

    return a.run();
}
