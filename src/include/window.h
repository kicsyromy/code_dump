#include <vulkan.h>

#include <memory>

#include "global.h"


namespace vulkan_tutorial
{
    struct window_handle_t;

    class window_t
    {
    public:
        window_t(
            std::string_view window_title,
            int x, int y,
            int width, int height) noexcept;
        ~window_t() noexcept;

    public:
        DEFAULT_MOVE(window_t)

    public:
        void run_render_loop() const noexcept;

    private:
        window_handle_t *handle_ { nullptr };

    private:
        DISABLE_COPY(window_t)
    };
}
