#ifndef SUPPORT_H
#define SUPPORT_H

#include <unordered_map>
#include <vector>
#include <array>
#include <algorithm>

#include <cstring>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace vk::support
{
    inline const std::unordered_map<VkResult, const char * const> VK_RESULT_STRING {
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

    std::vector<VkExtensionProperties> supported_extensions()
    {
        std::uint32_t extension_count { 0 };
        vkEnumerateInstanceExtensionProperties(
            nullptr, &extension_count, nullptr
        );

        std::vector<VkExtensionProperties> extensions(extension_count);
        vkEnumerateInstanceExtensionProperties(
            nullptr, &extension_count, extensions.data()
        );

        return extensions;
    }

    template <std::size_t count>
    bool check_validation_layer_support(
            const std::array<const char *, count> &requested_layers)
    {
        std::uint32_t layer_count { 0 };
        vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

        std::vector<VkLayerProperties> available_layers(layer_count);
        vkEnumerateInstanceLayerProperties(
            &layer_count, available_layers.data()
        );

        bool result { false };
        for (const auto &requested_layer: requested_layers)
        {
            auto it = std::find_if(
                        available_layers.cbegin(), available_layers.cend(),
                        [&requested_layer](const auto &available_layer) {
                return (strcmp(requested_layer, available_layer.layerName) == 0);
            });

            if (it != available_layers.cend())
            {
                result = true;
                break;
            }
        }

        return result;
    }
}

#define DEFAULT_COPY(klass)                                                    \
    klass(const klass &) = default;                                            \
    klass &operator=(const klass &) = default;

#define DISABLE_COPY(klass)                                                    \
    klass(const klass &) = delete;                                             \
    klass &operator=(const klass &) = delete;

#define DEFAULT_MOVE(klass)                                                    \
    klass(klass &&) = default;                                                 \
    klass &operator=(klass &&) = default;

#define DISABLE_MOVE(klass)                                                    \
    klass(klass &&) = delete;                                                  \
    klass &operator=(klass &&) = delete;

#endif // !SUPPORT_H
