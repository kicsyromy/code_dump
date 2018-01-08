#ifndef VULKAN_INSTANCE_H
#define VULKAN_INSTANCE_H

#include <vector>
#include <string_view>

#include <vulkan.h>

#include "global.h"
#include "physical_device.h"

namespace vulkan_tutorial
{
    class vulkan_instance_t
    {
    public:
        static std::string_view vk_result_to_string(VkResult value) noexcept;

    public:
        static std::vector<VkExtensionProperties>
            available_extensions() noexcept;

        static bool extension_available(
                std::string_view desired_extension) noexcept;

    public:
        vulkan_instance_t(
                const char *application_name,
                std::uint32_t application_version,
                const char *engine_name,
                std::uint32_t engine_version,
                const std::vector<const char *> &enabled_layers,
                const std::vector<const char *> &enabled_extensions)
        noexcept;

        ~vulkan_instance_t() noexcept;

    public:
        DEFAULT_MOVE(vulkan_instance_t)

    public:
        std::vector<physical_device_t> physical_devices() const;
        inline bool valid() noexcept { return handle_ != nullptr; }

    public:
        inline operator VkInstance() noexcept { return handle_; }
        inline operator const VkInstance() const noexcept { return handle_; }

    private:
        VkInstance handle_ { nullptr };

    private:
        DISABLE_COPY(vulkan_instance_t)
    };
}

#endif // !VULKAN_INSTANCE_H
