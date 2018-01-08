#ifndef VK_TUTORIAL_QUEUE_H
#define VK_TUTORIAL_QUEUE_H

#include <vulkan.h>

#include "global.h"

namespace vulkan_tutorial
{
    class queue_t
    {
    public:
        queue_t() noexcept = default;
        ~queue_t() noexcept = default;

    public:
        DEFAULT_MOVE(queue_t)

    public:
        inline bool valid() const noexcept { return handle_ != nullptr; }

    public:
        inline operator VkQueue() noexcept { return handle_; }
        inline operator const VkQueue() const noexcept { return handle_; }

    private:
        VkQueue handle_ { nullptr };

    private:
        DISABLE_COPY(queue_t)
    };
}

#endif // !K_TUTORIAL_QUEUE_H
