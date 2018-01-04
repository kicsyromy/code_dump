#ifndef VK_RENDER_PASS_H
#define VK_RENDER_PASS_H

#include "support.h"
#include "vk_logical_device.h"
#include "logger.h"

namespace vk
{
    class render_pass_t
    {
    public:
        template<
                typename attachment_array_t,
                typename subpass_array_t,
                typename subpass_dependency_array_t>
        inline render_pass_t(
            const vk::logical_device_t &device,
            attachment_array_t &&attachments,
            subpass_array_t &&subpasses,
            subpass_dependency_array_t &&subpass_dependencies)
          : vk_logical_device_(device)
        {
            using namespace vk::support;

            const VkRenderPassCreateInfo create_info {
                VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
                nullptr,
                0,
                static_cast<std::uint32_t>(attachments.size()),
                attachments.data(),
                static_cast<std::uint32_t>(subpasses.size()),
                subpasses.data(),
                static_cast<std::uint32_t>(subpass_dependencies.size()),
                subpass_dependencies.data()
            };

            if (auto result = vkCreateRenderPass(
                        device,
                        &create_info,
                        nullptr,
                        &handle_); result != VK_SUCCESS)
            {
                LOG_FATAL("Failed to create render pass. Error: %s",
                          VK_RESULT_STRING.at(result));
            }
        }

        inline ~render_pass_t()
        {
            vkDestroyRenderPass(vk_logical_device_, handle_, nullptr);
        }

    public:
        inline operator VkRenderPass() const { return handle_; }

    private:
        VkRenderPass handle_ { nullptr };
        const vk::logical_device_t &vk_logical_device_;

    private:
        DISABLE_COPY(render_pass_t)
        DISABLE_MOVE(render_pass_t)
    };
}

#endif // !VK_RENDER_PASS_H
