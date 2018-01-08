#ifdef VK_USE_PLATFORM_XCB_KHR
#include "window.h"

#include <unistd.h>

namespace vulkan_tutorial
{
    struct window_handle_t
    {
    };
}

using namespace vulkan_tutorial;

window_t::window_t(
    std::string_view window_title,
    int x, int y,
    int width, int height) noexcept
  : handle_ { new window_handle_t }
{
}

window_t::~window_t() noexcept
{
    delete handle_;
}

void window_t::run_render_loop() const noexcept
{
    for (;;)
    {
        sleep(1000);
    }
}

#endif // VK_USE_PLATFORM_XCB_KHR
