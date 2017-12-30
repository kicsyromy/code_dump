#ifndef VK_PHYSICAL_DEVICE_H
#define VK_PHYSICAL_DEVICE_H

#include <vector>
#include <array>
#include <set>
#include <string_view>

#include <cstdint>

#include "support.h"
#include "logger.h"
#include "vk_instance.h"
#include "vk_surface.h"

namespace vk
{
    class physical_device_t
    {
    public:
        static std::vector<physical_device_t> physical_devices(
                const vk::instance_t &vk_instance)
        {
            std::uint32_t device_count { 0 };
            vkEnumeratePhysicalDevices(vk_instance, &device_count, nullptr);

            std::vector<VkPhysicalDevice> result(device_count);

            if (device_count != 0)
            {
                vkEnumeratePhysicalDevices(
                            vk_instance, &device_count, result.data());
            }

            return { result.begin(), result.end() };
        }

    public:
        inline physical_device_t(VkPhysicalDevice &device_handle)
          : handle_(device_handle) {}
        inline ~physical_device_t() = default;

    public:
        inline VkPhysicalDeviceProperties properties() const
        {
            VkPhysicalDeviceProperties result { };
            vkGetPhysicalDeviceProperties(handle_, &result);
            return result;
        }

        inline VkPhysicalDeviceFeatures   features() const
        {
            VkPhysicalDeviceFeatures result { };
            vkGetPhysicalDeviceFeatures(handle_, &result);
            return result;
        }

        inline std::vector<VkExtensionProperties> supported_extensions() const
        {
            std::uint32_t extension_count { 0 };
            vkEnumerateDeviceExtensionProperties(
                        handle_, nullptr, &extension_count, nullptr);

            std::vector<VkExtensionProperties> result(extension_count);

            vkEnumerateDeviceExtensionProperties(
                        handle_, nullptr, &extension_count, result.data());

            return result;
        }

        template <std::size_t count>
        inline bool extensions_supported(
                const std::array<const char *, count> &required_extensions) const
        {
            auto available_extensions = supported_extensions();
            std::set<std::string_view> req_ext(
                required_extensions.cbegin(),
                required_extensions.cend()
            );

            for (const auto &extension: available_extensions)
            {
                req_ext.erase(extension.extensionName);
                if (req_ext.empty())
                {
                    break;
                }
            }

            return req_ext.empty();
        }

        inline std::vector<VkQueueFamilyProperties> queue_families() const
        {
            std::uint32_t count { 0 };
            vkGetPhysicalDeviceQueueFamilyProperties(handle_, &count, nullptr);

            std::vector<VkQueueFamilyProperties> result(count);
            vkGetPhysicalDeviceQueueFamilyProperties(
                        handle_, &count, result.data());

            return result;
        }

        inline int queue_family_index(VkQueueFlags flags) const
        {
            int result { -1 };
            int index  {  0 };

            for (const auto &queue_family: queue_families())
            {
                if ((queue_family.queueCount > 0)
                        && (queue_family.queueFlags & flags))
                {
                    result = index;
                    break;
                }

                ++index;
            }

            return result;
        }

        inline int queue_surface_support_index(
                const vk::surface_t &surface) const
        {
            int result { -1 };
            int index  {  0 };

            for (const auto &queue_family: queue_families())
            {
                VkBool32 present_support { false };
                vkGetPhysicalDeviceSurfaceSupportKHR(
                            handle_, index, surface, &present_support);

                if ((queue_family.queueCount > 0)
                        && present_support)
                {
                    result = index;
                    break;
                }

                ++index;
            }

            return result;
        }

        inline VkSurfaceCapabilitiesKHR surface_capabilities(
                const vk::surface_t &surface) const
        {
            VkSurfaceCapabilitiesKHR result { };

            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
                        handle_, surface, &result);

            return result;
        }

        /* TODO: Move these to vk::surface_t */
        inline std::vector<VkSurfaceFormatKHR> surface_formats(
                const vk::surface_t &surface) const
        {
            std::uint32_t count;
            vkGetPhysicalDeviceSurfaceFormatsKHR(
                        handle_, surface, &count, nullptr);

            std::vector<VkSurfaceFormatKHR> result(count);
            vkGetPhysicalDeviceSurfaceFormatsKHR(
                        handle_, surface, &count, result.data());

            return result;
        }

        inline std::vector<VkPresentModeKHR> surface_present_modes(
                const vk::surface_t &surface) const
        {
            std::uint32_t count;
            vkGetPhysicalDeviceSurfacePresentModesKHR(
                        handle_, surface, &count, nullptr);

            std::vector<VkPresentModeKHR> result(count);
            vkGetPhysicalDeviceSurfacePresentModesKHR(
                        handle_, surface, &count, result.data());

            return result;
        }

    public:
        inline operator VkPhysicalDevice() const { return handle_; }

    private:
        VkPhysicalDevice handle_ { nullptr };

    public:
        DEFAULT_MOVE(physical_device_t)

    private:
        DISABLE_COPY(physical_device_t)
    };
}

#endif // !VK_PHYSICAL_DEVICE_H
