#ifndef VK_COMMAND_BUFFER_H
#define VK_COMMAND_BUFFER_H

#include "support.h"
#include "vk_logical_device.h"
#include "vk_command_pool.h"
#include "logger.h"

namespace vk
{
    class command_buffer_t
    {
    public:
        inline command_buffer_t(
                const vk::logical_device_t &device,
                const vk::command_pool_t &command_pool,
                std::size_t size)
        {
            using namespace vk::support;

            buffer_.resize(size);

            const VkCommandBufferAllocateInfo allocate_info {
                VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                nullptr,
                command_pool,
                VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                static_cast<std::uint32_t>(size)
            };

            if (auto result =
                    vkAllocateCommandBuffers(
                        device,
                        &allocate_info,
                        buffer_.data()); result != VK_SUCCESS)
            {
                LOG_FATAL("Failed to allocate command buffers. Error: %s",
                          VK_RESULT_STRING.at(result));
            }
        }

        inline ~command_buffer_t() = default;

    public:
        inline operator std::vector<VkCommandBuffer> &()
        {
            return buffer_;
        }

        inline operator const std::vector<VkCommandBuffer> &() const
        {
            return buffer_;
        }

    private:
        std::vector<VkCommandBuffer> buffer_ { };

    private:
        DISABLE_COPY(command_buffer_t)
        DISABLE_MOVE(command_buffer_t)
    };
}

#endif // !VK_COMMAND_BUFFER_H
