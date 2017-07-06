#include "gulp_window.h"
#include "gulp_window_p.h"

using namespace gulp;

Window::Window()
  : priv_{ std::make_unique<WindowPrivate>() }
{
}
