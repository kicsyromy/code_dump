#ifndef VK_FRAMEBUFFER_H
#define VK_FRAMEBUFFER_H

#include <vector>
#include <memory>

#include "support.h"
#include "vk_logical_device.h"
#include "vk_render_pass.h"
#include "vk_image_view.h"
#include "logger.h"

namespace vk
{
    class framebuffer_t
    {
    public:
        inline framebuffer_t(
                const vk::logical_device_t &device,
                const vk::render_pass_t &render_pass,
                std::uint32_t width, std::uint32_t height,
                const std::vector<vk::image_view_t *> &image_views)
          : vk_logical_device_(device)
        {
            using namespace vk::support;

            std::vector<VkImageView> image_view_list;
            image_view_list.reserve(image_views.size());

            for (const auto &image_view: image_views)
            {
                image_view_list.push_back(*image_view);
            }

            const VkFramebufferCreateInfo create_info {
                VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                nullptr,
                0,
                render_pass,
                static_cast<std::uint32_t>(image_view_list.size()),
                image_view_list.data(),
                width,
                height,
                1
            };

            if (auto result =
                    vkCreateFramebuffer(
                        device,
                        &create_info,
                        nullptr,
                        &handle_); result != VK_SUCCESS)
            {

                LOG_FATAL("Failed to create framebuffer. Error: %s",
                          VK_RESULT_STRING.at(result));
            }
        }

        inline ~framebuffer_t()
        {
            vkDestroyFramebuffer(vk_logical_device_, handle_, nullptr);
        }

    public:
        inline operator VkFramebuffer() const { return handle_; }

    private:
        VkFramebuffer handle_ { nullptr };
        const vk::logical_device_t &vk_logical_device_;

    private:
        DISABLE_COPY(framebuffer_t)
        DISABLE_MOVE(framebuffer_t)
    };
}

#endif // !VK_FRAMEBUFFER_H
