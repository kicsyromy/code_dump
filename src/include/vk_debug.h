#ifndef VK_DEBUG_H
#define VK_DEBUG_H

#include "vk_instance.h"
#include "support.h"
#include "logger.h"

namespace vk
{
    class debug_t
    {
    private:
        static  VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
            VkDebugReportFlagsEXT /* flags */,
            VkDebugReportObjectTypeEXT /* object_type */,
            std::uint64_t /* object */,
            std::size_t /* location */,
            std::int32_t /* code */,
            const char *layer_prefix,
            const char *message,
            void * /* user_data */)
        {
            LOG_ERROR("Validation Layer: %s: %s", layer_prefix, message);
            return VK_FALSE;
        }

        VkResult create_debug_report_callback_ext(
                const VkDebugReportCallbackCreateInfoEXT &create_info)
        {
            auto function = (PFN_vkCreateDebugReportCallbackEXT)
                    vkGetInstanceProcAddr(
                        vk_instance_,
                        "vkCreateDebugReportCallbackEXT");

            if (function != nullptr)
            {
                return function(vk_instance_, &create_info, nullptr, &handle_);
            }
            else
            {
                return VK_ERROR_EXTENSION_NOT_PRESENT;
            }
        }

    public:
        inline debug_t(vk::instance_t &vk_instance)
          : vk_instance_(vk_instance)
        {
            VkDebugReportCallbackCreateInfoEXT create_info { };
            create_info.sType =
                    VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
            create_info.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT
                    | VK_DEBUG_REPORT_WARNING_BIT_EXT;
            create_info.pfnCallback = &debug_callback;
            create_debug_report_callback_ext(create_info);
        }

        inline ~debug_t()
        {
            auto function = (PFN_vkDestroyDebugReportCallbackEXT)
                    vkGetInstanceProcAddr(
                        vk_instance_,
                        "vkDestroyDebugReportCallbackEXT");
            if (function)
            {
                function(vk_instance_, handle_, nullptr);
            }
        }

    private:
        VkDebugReportCallbackEXT handle_ { nullptr };
        vk::instance_t &vk_instance_;

    private:
        DISABLE_COPY(debug_t)
        DISABLE_MOVE(debug_t)
    };
}

#endif // !VK_DEBUG_H
