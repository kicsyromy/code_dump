#ifndef VK_SAHDER_H
#define VK_SAHDER_H

#include <memory>
#include <vector>
#include <unordered_map>

#include <cstdio>
#include <cstring>

#include "support.h"
#include "vk_logical_device.h"
#include "logger.h"

namespace vk
{
    class shader_t
    {
        using file_handle_t = std::unique_ptr<FILE, decltype(&std::fclose)>;

    public:
        using byte_t = std::uint8_t;

        enum class ShaderType
        {
            Vertex   = VK_SHADER_STAGE_VERTEX_BIT,
            Geometry = VK_SHADER_STAGE_GEOMETRY_BIT,
            Fragment = VK_SHADER_STAGE_FRAGMENT_BIT
        };

    public:
        inline shader_t(
                const vk::logical_device_t &device,
                ShaderType type,
                const char *file_path)
          : vk_logical_device_(device)
        {
            {
                file_handle_t shader(std::fopen(file_path, "rb"), &std::fclose);
                if (!shader)
                {
                    LOG_FATAL(
                        "Failed to open %s. %s", file_path,
                        std::strerror(errno)
                    );
                }

                fseek(shader.get(), 0, SEEK_END);
                auto file_size = static_cast<std::size_t>(ftell(shader.get()));
                fseek(shader.get(), 0, SEEK_SET);

                data_.resize(file_size);
                fread(data_.data(), file_size, 1, shader.get());
            }

            const VkShaderModuleCreateInfo create_info {
                VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
                nullptr,
                0,
                data_.size(),
                reinterpret_cast<const std::uint32_t *>(data_.data())
            };

            if (vkCreateShaderModule(device, &create_info, nullptr, &handle_)
                    != VK_SUCCESS)
            {
                LOG_FATAL("Failed to create shader module");
            }

            pipeline_shader_stage_.stage = static_cast<VkShaderStageFlagBits>(type);
            pipeline_shader_stage_.module = handle_;
        }

        inline ~shader_t()
        {
            vkDestroyShaderModule(vk_logical_device_, handle_, nullptr);
        }

    public:
        inline const VkPipelineShaderStageCreateInfo &stage_info() const
        {
            return pipeline_shader_stage_;
        }

        inline const std::vector<byte_t> &data() const
        {
            return data_;
        }

    public:
        inline operator VkShaderModule() const { return handle_; }

    private:
        VkShaderModule handle_ { nullptr };
        VkPipelineShaderStageCreateInfo pipeline_shader_stage_ {
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            nullptr,
            0,
            VK_SHADER_STAGE_FRAGMENT_BIT,
            nullptr,
            "main",
            nullptr
        };
        std::vector<byte_t> data_ { };
        const vk::logical_device_t &vk_logical_device_;
    };
}

#endif // !VK_SAHDER_H
