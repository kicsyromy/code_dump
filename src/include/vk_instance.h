#ifndef VK_INSTANCE_H
#define VK_INSTANCE_H

#include <optional>
#include <functional>
#include <type_traits>

#include "support.h"

namespace vk
{
    class instance_t
    {
    public:
        template <typename array_t = nullptr_t>
        inline instance_t(
                const char *app_name,
                const std::vector<const char *> &enabled_extensions,
                const array_t &validation_layers = nullptr)
        {
            using namespace support;

            VkApplicationInfo app_info {
                VK_STRUCTURE_TYPE_APPLICATION_INFO,
                nullptr,
                app_name,
                VK_MAKE_VERSION(1, 0, 0),
                "No Engine",
                VK_MAKE_VERSION(1, 0, 0),
                VK_API_VERSION_1_0
            };

            VkInstanceCreateInfo create_info { };
            create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            create_info.pApplicationInfo = &app_info;
            create_info.enabledExtensionCount = enabled_extensions.size();
            create_info.ppEnabledExtensionNames = enabled_extensions.data();

            if constexpr (std::is_same_v<array_t, nullptr_t>)
            {
                create_info.enabledLayerCount = 0;
            }
            else
            {
                create_info.enabledLayerCount = validation_layers.size();
                create_info.ppEnabledLayerNames = validation_layers.data();
            }

            if (auto result = vkCreateInstance(&create_info, nullptr, &handle_); result != VK_SUCCESS)
            {
                LOG_FATAL("Failed to create Vulkan instance. Error: %s", VK_RESULT_STRING.at(result));
            }
        }

        inline ~instance_t() { vkDestroyInstance(handle_, nullptr); }

    public:
        inline operator VkInstance() const { return handle_; }

    public:
        DEFAULT_MOVE(instance_t)

    private:
        VkInstance handle_ { nullptr };

    private:
        DISABLE_COPY(instance_t)
    };
}

#endif // !VK_INSTANCE_H
