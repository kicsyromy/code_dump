#ifndef VK_PIPELINE_H
#define VK_PIPELINE_H

#include "support.h"
#include "vk_logical_device.h"
#include "vk_render_pass.h"
#include "vk_pipeline_layout.h"
#include "logger.h"

namespace vk
{
    class pipeline_t
    {
    public:
        template<typename shader_stage_array_t>
        inline pipeline_t(
                const vk::logical_device_t &device,
                shader_stage_array_t &&shader_stages,
                const VkPipelineVertexInputStateCreateInfo &vertex_input,
                const VkPipelineInputAssemblyStateCreateInfo &input_assembly,
                const VkPipelineViewportStateCreateInfo &viewport_state,
                const VkPipelineRasterizationStateCreateInfo &rasterizer,
                const VkPipelineMultisampleStateCreateInfo &multisampling,
//                const VkPipelineDepthStencilStateCreateInfo &depth_stencil
                const VkPipelineColorBlendStateCreateInfo &color_blending,
                const VkPipelineDynamicStateCreateInfo &dynamic_states,
                const vk::render_pass_t &render_pass,
                const vk::pipeline_layout_t &pipeline_layout)
          : vk_logical_device_(device)
        {
            using namespace vk::support;

            const VkGraphicsPipelineCreateInfo create_info {
                VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
                nullptr,
                0,
                static_cast<std::uint32_t>(shader_stages.size()),
                shader_stages.data(),
                &vertex_input,
                &input_assembly,
                nullptr,
                &viewport_state,
                &rasterizer,
                &multisampling,
                nullptr,
                &color_blending,
                &dynamic_states,
                pipeline_layout,
                render_pass,
                0,
                VK_NULL_HANDLE,
                -1
            };

            if (auto result =
                    vkCreateGraphicsPipelines(
                        device,
                        VK_NULL_HANDLE,
                        1,
                        &create_info,
                        nullptr,
                        &handle_); result != VK_SUCCESS)
            {

                LOG_FATAL("Failed to create graphics pipeline. Error: %s",
                          VK_RESULT_STRING.at(result));
            }
        }

        inline ~pipeline_t()
        {
            vkDestroyPipeline(vk_logical_device_, handle_, nullptr);
        }

    public:
        inline operator VkPipeline() const { return handle_; }

    private:
        VkPipeline handle_ { nullptr };
        const vk::logical_device_t &vk_logical_device_;

    private:
        DISABLE_COPY(pipeline_t)
        DISABLE_MOVE(pipeline_t)
    };
}

#endif // !VK_PIPELINE_H
