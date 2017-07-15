#include "gulp_window.h"
#include "gulp_platform_window_p.h"

#include "gulp_platform_p.h"

using namespace gulp;

Window::Window()
  : priv_{ nullptr }
{
    auto init = Platform::getInitializers();
    priv_.reset(init.window_new());
}

void Window::show()
{
    priv_->show();
}
