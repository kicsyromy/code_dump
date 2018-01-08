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
        template<
            typename layer_array_t,
            typename extension_array_t
        >
        vulkan_instance_t(
                const char *application_name,
                std::uint32_t application_version,
                const char *engine_name,
                std::uint32_t engine_version,
                const layer_array_t &enabled_layers,
                const extension_array_t &enabled_extensions)
        noexcept;

        ~vulkan_instance_t() noexcept;

    public:
        DEFAULT_MOVE(vulkan_instance_t)

    public:
        std::vector<physical_device_t> physical_devices() const noexcept;

        inline const std::vector<std::string_view> &
        enabled_layers() const noexcept { return enabled_layers_; }

        inline const std::vector<std::string_view> &
        enabled_extensions() const noexcept { return enabled_extensions_; }

        inline bool valid() noexcept { return handle_ != nullptr; }

    public:
        inline operator VkInstance() noexcept { return handle_; }
        inline operator const VkInstance() const noexcept { return handle_; }

    private:
        void initialize_handle(const VkInstanceCreateInfo &create_info) noexcept;

    private:
        VkInstance handle_ { nullptr };
        const std::vector<std::string_view> enabled_layers_;
        const std::vector<std::string_view> enabled_extensions_;

    private:
        DISABLE_COPY(vulkan_instance_t)
    };

    template<
        typename layer_array_t,
        typename extension_array_t
    >
    vulkan_instance_t::vulkan_instance_t(
            const char *application_name,
            std::uint32_t application_version,
            const char *engine_name,
            std::uint32_t engine_version,
            const layer_array_t &enabled_layers,
            const extension_array_t &enabled_extensions) noexcept
      : enabled_layers_ { enabled_layers.cbegin(), enabled_layers.cend() }
      , enabled_extensions_ {
            enabled_extensions.cbegin(), enabled_extensions.cend()
        }
    {
        const VkApplicationInfo app_info {
            VK_STRUCTURE_TYPE_APPLICATION_INFO,
            nullptr,
            application_name,
            application_version,
            engine_name,
            engine_version,
            VK_MAKE_VERSION(1, 0, 0)
        };

        const VkInstanceCreateInfo create_info {
            VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            nullptr,
            0,
            &app_info,
            static_cast<std::uint32_t>(enabled_layers.size()),
            enabled_layers.data(),
            static_cast<std::uint32_t>(enabled_extensions.size()),
            enabled_extensions.data()
        };

        initialize_handle(create_info);
    }
}

#endif // !VULKAN_INSTANCE_H
