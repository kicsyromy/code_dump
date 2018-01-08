#include "vulkan_instance.h"

#include <unordered_map>
#include <algorithm>

#include <cstdint>

#include "vulkan_functions.h"
#include "logger.h"

using namespace vulkan_tutorial;

namespace
{
    const std::unordered_map<VkResult, std::string_view> VK_RESULT_STRING {
        { VK_SUCCESS,                           "VK_SUCCESS"                           },
        { VK_NOT_READY,                         "VK_NOT_READY"                         },
        { VK_TIMEOUT,                           "VK_TIMEOUT"                           },
        { VK_EVENT_SET,                         "VK_EVENT_SET"                         },
        { VK_EVENT_RESET,                       "VK_EVENT_RESET"                       },
        { VK_INCOMPLETE,                        "VK_INCOMPLETE"                        },
        { VK_ERROR_OUT_OF_HOST_MEMORY,          "VK_ERROR_OUT_OF_HOST_MEMORY"          },
        { VK_ERROR_OUT_OF_DEVICE_MEMORY,        "VK_ERROR_OUT_OF_DEVICE_MEMORY"        },
        { VK_ERROR_INITIALIZATION_FAILED,       "VK_ERROR_INITIALIZATION_FAILED"       },
        { VK_ERROR_DEVICE_LOST,                 "VK_ERROR_DEVICE_LOST"                 },
        { VK_ERROR_MEMORY_MAP_FAILED,           "VK_ERROR_MEMORY_MAP_FAILED"           },
        { VK_ERROR_LAYER_NOT_PRESENT,           "VK_ERROR_LAYER_NOT_PRESENT"           },
        { VK_ERROR_EXTENSION_NOT_PRESENT,       "VK_ERROR_EXTENSION_NOT_PRESENT"       },
        { VK_ERROR_FEATURE_NOT_PRESENT,         "VK_ERROR_FEATURE_NOT_PRESENT"         },
        { VK_ERROR_INCOMPATIBLE_DRIVER,         "VK_ERROR_INCOMPATIBLE_DRIVER"         },
        { VK_ERROR_TOO_MANY_OBJECTS,            "VK_ERROR_TOO_MANY_OBJECTS"            },
        { VK_ERROR_FORMAT_NOT_SUPPORTED,        "VK_ERROR_FORMAT_NOT_SUPPORTED"        },
        { VK_ERROR_FRAGMENTED_POOL,             "VK_ERROR_FRAGMENTED_POOL"             },
        { VK_ERROR_SURFACE_LOST_KHR,            "VK_ERROR_SURFACE_LOST_KHR"            },
        { VK_ERROR_NATIVE_WINDOW_IN_USE_KHR,    "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR"    },
        { VK_SUBOPTIMAL_KHR,                    "VK_SUBOPTIMAL_KHR"                    },
        { VK_ERROR_OUT_OF_DATE_KHR,             "VK_ERROR_OUT_OF_DATE_KHR"             },
        { VK_ERROR_INCOMPATIBLE_DISPLAY_KHR,    "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR"    },
        { VK_ERROR_VALIDATION_FAILED_EXT,       "VK_ERROR_VALIDATION_FAILED_EXT"       },
        { VK_ERROR_INVALID_SHADER_NV,           "VK_ERROR_INVALID_SHADER_NV"           },
        { VK_ERROR_OUT_OF_POOL_MEMORY_KHR,      "VK_ERROR_OUT_OF_POOL_MEMORY_KHR"      },
        { VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR, "VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR" },
        { VK_ERROR_NOT_PERMITTED_EXT,           "VK_ERROR_NOT_PERMITTED_EXT"           }
    };
}

std::string_view vulkan_instance_t::vk_result_to_string(VkResult value) noexcept
{
    auto result = std::string_view { };

    auto it = VK_RESULT_STRING.find(value);
    if (it != VK_RESULT_STRING.cend())
    {
        result = it->second;
    }

    return result;
}

std::vector<VkExtensionProperties>
vulkan_instance_t::available_extensions() noexcept
{
    std::uint32_t count;
    vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);

    std::vector<VkExtensionProperties> result(count);
    vkEnumerateInstanceExtensionProperties(nullptr, &count, result.data());

    return result;
}

bool vulkan_instance_t::extension_available(
        std::string_view desired_extension) noexcept
{
    static const auto extensions = available_extensions();

    auto it = std::find_if(
        extensions.cbegin(),
        extensions.cend(),
        [&desired_extension](const auto &extension) {
            return (desired_extension == extension.extensionName);
        }
    );

    return (it != extensions.cend());
}

vulkan_instance_t::vulkan_instance_t(
        const char *application_name,
        std::uint32_t application_version,
        const char *engine_name,
        std::uint32_t engine_version,
        const std::vector<const char *> &enabled_layers,
        const std::vector<const char *> &enabled_extensions) noexcept
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

    if (auto r = vkCreateInstance(&create_info, nullptr, &handle_);
             r != VK_SUCCESS)
    {
        handle_ = nullptr;
        LOG_ERROR("Failed to create Vulkan instance. Error: %s",
                  vk_result_to_string(r).data());
    }
}

vulkan_instance_t::~vulkan_instance_t() noexcept
{
    if (valid())
    {
        vkDestroyInstance(handle_, nullptr);
    }
}

std::vector<physical_device_t> vulkan_instance_t::physical_devices() const
{
    std::uint32_t count { 0 };
    vkEnumeratePhysicalDevices(handle_, &count, nullptr);

    std::vector<physical_device_t> result(count);
    vkEnumeratePhysicalDevices(
                handle_,
                &count,
                reinterpret_cast<VkPhysicalDevice *>(result.data()));

    return result;
}
