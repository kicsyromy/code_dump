#include "triangle_app.h"

#include <array>
#include <algorithm>
#include <memory>

#include <cstdint>

#include "support.h"
#include "logger.h"

#include "vk_instance.h"
#include "vk_window.h"
#include "vk_debug.h"
#include "vk_surface.h"
#include "vk_physical_device.h"
#include "vk_logical_device.h"
#include "vk_queue.h"
#include "vk_swap_chain.h"
#include "vk_image_view.h"
#include "vk_shader.h"
#include "vk_render_pass.h"
#include "vk_pipeline_layout.h"
#include "vk_pipeline.h"
#include "vk_framebuffer.h"
#include "vk_command_pool.h"
#include "vk_command_buffer.h"
#include "vk_semaphore.h"

using namespace triangle_application;

namespace
{
#ifndef NDEBUG
    constexpr const std::array validation_layers {
        "VK_LAYER_LUNARG_standard_validation"
    };
#endif

    const std::array required_device_extensions {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    std::vector<const char *> required_extensions()
    {
        std::vector<const char *> result;

        std::uint32_t glfw_extension_count { 0 };
        auto glfw_extensions =
                glfwGetRequiredInstanceExtensions(&glfw_extension_count);

        for (std::uint32_t it = 0; it < glfw_extension_count; ++it)
        {
            result.push_back(glfw_extensions[it]);
        }

#ifndef NDEBUG
        result.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#endif

        return result;
    }

    vk::instance_t create_instance()
    {
#ifndef NDEBUG
        fprintf(stderr, "Available Vulkan extensions:\n");
        const auto extensions = vk::support::supported_extensions();
        for (const auto &extension: extensions)
        {
            fprintf(stderr, "    %s\n", extension.extensionName);
        }

        if (!vk::support::check_validation_layer_support(validation_layers))
        {
            LOG_FATAL("Requested validation layers are unavailable");
        }

        return {
            "Triangle Application",
            required_extensions(),
            validation_layers
        };
#else
        return {
            "Triangle Application",
            required_extensions()
        };
#endif
    }

    bool is_device_suitable(
            const vk::physical_device_t &device,
            const vk::surface_t &surface)
    {
        auto queue_families = device.queue_families();

        int index { 0 };
        auto it = std::find_if(queue_families.cbegin(), queue_families.cend(),
                               [&](const auto &queue_family)
        {
            VkBool32 present_support { false };
            vkGetPhysicalDeviceSurfaceSupportKHR(
                        device,
                        static_cast<std::uint32_t>(index),
                        surface,
                        &present_support);
            ++index;

            return ((queue_family.queueCount > 0) && present_support &&
                (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT));
        });

        return (
            (it != queue_families.cend()) &&
            (device.extensions_supported(required_device_extensions)) &&
            !device.surface_formats(surface).empty() &&
            !device.surface_present_modes(surface).empty()
        );
    }

    vk::physical_device_t pick_physical_device(
            const vk::instance_t &vk_instance,
            const vk::surface_t &vk_surface)
    {
        auto devices = vk::physical_device_t::physical_devices(vk_instance);

        for (auto &device: devices)
        {
            if (is_device_suitable(device, vk_surface))
            {
                VkPhysicalDevice dev = device;
                return { dev };
            }
        }

        LOG_FATAL("Failed to find a suitable Vulkan capable GPU");
    }

    VkSurfaceFormatKHR choose_swap_surface_format(
            const vk::physical_device_t &device,
            const vk::surface_t &surface)
    {
        static constexpr auto PREFERED_FORMAT {
            VK_FORMAT_B8G8R8A8_UNORM
        };
        static constexpr auto PREFERED_COLOR_SPACE {
            VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
        };

        VkSurfaceFormatKHR result {
            PREFERED_FORMAT,
            PREFERED_COLOR_SPACE
        };

        auto available_formats = device.surface_formats(surface);

        if (!((available_formats.size() == 1) &&
              (available_formats.front().format == VK_FORMAT_UNDEFINED)))
        {
            auto it = std::find_if(
                          available_formats.cbegin(),
                          available_formats.cend(),
                          [] (const auto &format){
                              return (
                                  (format.format == PREFERED_FORMAT) &&
                                  (format.colorSpace == PREFERED_COLOR_SPACE)
                              );
                          }
                      );
            if (it == available_formats.cend())
            {
                result = available_formats.front();
            }
        }

        return result;
    }

    VkPresentModeKHR choose_swap_present_mode(
            const vk::physical_device_t &device,
            const vk::surface_t &surface)
    {
        VkPresentModeKHR result { VK_PRESENT_MODE_FIFO_KHR };

        auto available_modes = device.surface_present_modes(surface);
        for (const auto &mode: available_modes)
        {
            if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                result = mode;
                break;
            }

            if (mode == VK_PRESENT_MODE_IMMEDIATE_KHR)
            {
                result = mode;
                break;
            }
        }

        return result;
    }

    VkExtent2D choose_swap_extent(
            const vk::physical_device_t &device,
            const vk::surface_t &surface,
            const vk::window_t &window)
    {
        auto capabilities = device.surface_capabilities(surface);

        VkExtent2D result {
            capabilities.currentExtent.width,
            capabilities.currentExtent.height
        };

        if (capabilities.currentExtent.width
                == std::numeric_limits<std::uint32_t>::max())
        {
            result.width = std::max(
                               capabilities.minImageExtent.width,
                               std::min(
                                   capabilities.maxImageExtent.width,
                                   window.width()
                                )
                           );
            result.height = std::max(
                                capabilities.minImageExtent.height,
                                std::min(
                                    capabilities.maxImageExtent.height,
                                    window.height()
                                 )
                            );
        }

        return result;
    }

    vk::pipeline_t create_graphics_pipeline(
            const vk::logical_device_t &device,
            const vk::swap_chain_t &swap_chain,
            const vk::render_pass_t &render_pass,
            const vk::pipeline_layout_t &pipline_layout)
    {
        auto fragment_shader = vk::shader_t(
                    device,
                    vk::shader_t::ShaderType::Fragment,
                    SHADER_PATH"/frag.spv");
        auto vertex_shader   = vk::shader_t(
                    device,
                    vk::shader_t::ShaderType::Vertex,
                    SHADER_PATH"/vert.spv");

        const std::array shader_stages {
            fragment_shader.stage_info(),
            vertex_shader.stage_info()
        };

        const VkPipelineVertexInputStateCreateInfo vertex_input_info {
            VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            nullptr,
            0,
            0,
            nullptr,
            0,
            nullptr
        };

        const VkPipelineInputAssemblyStateCreateInfo input_assembly {
            VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            nullptr,
            0,
            VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            VK_FALSE
        };

        const VkViewport viewport {
            0.0f,
            0.0f,
            static_cast<float>(swap_chain.extent().width),
            static_cast<float>(swap_chain.extent().height),
            0.0f,
            1.0f
        };

        const VkRect2D scissor {
            { 0, 0 },
            swap_chain.extent()
        };

        const VkPipelineViewportStateCreateInfo viewport_state {
            VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            nullptr,
            0,
            1,
            &viewport,
            1,
            &scissor
        };

        const VkPipelineRasterizationStateCreateInfo rasterizer {
            VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            nullptr,
            0,
            VK_FALSE,
            VK_FALSE,
            VK_POLYGON_MODE_FILL,
            VK_CULL_MODE_BACK_BIT,
            VK_FRONT_FACE_CLOCKWISE,
            VK_FALSE,
            0.0f,
            0.0f,
            0.0f,
            1.0f
        };

        const VkPipelineMultisampleStateCreateInfo multisampling {
            VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            nullptr,
            0,
            VK_SAMPLE_COUNT_1_BIT,
            VK_FALSE,
            1.0f,
            nullptr,
            VK_FALSE,
            VK_FALSE
        };

        const VkPipelineColorBlendAttachmentState color_blend_attachment {
            VK_FALSE,
            VK_BLEND_FACTOR_ONE,
            VK_BLEND_FACTOR_ZERO,
            VK_BLEND_OP_ADD,
            VK_BLEND_FACTOR_ONE,
            VK_BLEND_FACTOR_ZERO,
            VK_BLEND_OP_ADD,
            VK_COLOR_COMPONENT_R_BIT |
            VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT |
            VK_COLOR_COMPONENT_A_BIT
        };

        const VkPipelineColorBlendStateCreateInfo color_blending {
            VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            nullptr,
            0,
            VK_FALSE,
            VK_LOGIC_OP_COPY,
            1,
            &color_blend_attachment,
            { 0.0f, 0.0f, 0.0f, 0.0f }
        };

        const std::array dynamic_states {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_LINE_WIDTH
        };

        const VkPipelineDynamicStateCreateInfo dynamic_state {
            VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            nullptr,
            0,
            2,
            dynamic_states.data()
        };

        return {
            device,
            shader_stages,
            vertex_input_info,
            input_assembly,
            viewport_state,
            rasterizer,
            multisampling,
            color_blending,
            render_pass,
            pipline_layout
        };
    }

    vk::render_pass_t create_render_pass(
            const vk::logical_device_t &device,
            const vk::swap_chain_t &swap_chain)
    {
        const VkAttachmentDescription color_attachment {
            0,
            swap_chain.format(),
            VK_SAMPLE_COUNT_1_BIT,
            VK_ATTACHMENT_LOAD_OP_CLEAR,
            VK_ATTACHMENT_STORE_OP_STORE,
            VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            VK_ATTACHMENT_STORE_OP_DONT_CARE,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
        };

        const VkAttachmentReference color_attachment_ref {
            0,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
        };

        VkSubpassDescription subpass { };
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &color_attachment_ref;

        const VkSubpassDependency dependency {
            VK_SUBPASS_EXTERNAL,
            0,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            0,
            VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            0
        };

        return {
            device,
            std::array { color_attachment },
            std::array { subpass },
            std::array { dependency }
        };
    }
}

void draw_frame(
        const vk::logical_device_t &device,
        const vk::swap_chain_t &swap_chain,
        const vk::command_buffer_t &command_buffer,
        const vk::queue_t &graphics_queue,
        const vk::queue_t &present_queue,
        vk::semaphore_t &image_available,
        vk::semaphore_t &render_finished)
{
    std::uint32_t image_index { 0 };
    vkAcquireNextImageKHR(
                device,
                swap_chain,
                std::numeric_limits<std::uint64_t>::max(),
                image_available,
                VK_NULL_HANDLE,
                &image_index);

    std::array wait_semaphores {
        static_cast<VkSemaphore>(image_available)
    };
    std::array signal_semaphores {
        static_cast<VkSemaphore>(render_finished)
    };
    std::array wait_stages {
        static_cast<VkFlags>(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT)
    };

    const VkSubmitInfo submit_info {
        VK_STRUCTURE_TYPE_SUBMIT_INFO,
        nullptr,
        static_cast<std::uint32_t>(wait_semaphores.size()),
        wait_semaphores.data(),
        wait_stages.data(),
        1,
        &command_buffer[image_index],
        static_cast<std::uint32_t>(signal_semaphores.size()),
        signal_semaphores.data()
    };

    if (auto result =
            vkQueueSubmit(graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
            result != VK_SUCCESS)
    {
        LOG_FATAL("Failed to submit draw command buffer. Error: %s",
                  vk::support::VK_RESULT_STRING.at(result));
    }

    std::array swap_chains {
        static_cast<VkSwapchainKHR>(swap_chain)
    };

    const VkPresentInfoKHR present_info {
        VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        nullptr,
        static_cast<std::uint32_t>(signal_semaphores.size()),
        signal_semaphores.data(),
        static_cast<std::uint32_t>(swap_chains.size()),
        swap_chains.data(),
        &image_index,
        nullptr
    };

    vkQueuePresentKHR(present_queue, &present_info);

    present_queue.wait_for_idle();
}

void triangle_application::run()
{
    vk::window_t window;
    auto vk_instance = create_instance();
#ifndef NDEBUG
    vk::debug_t d { vk_instance };
#endif
    auto surface = vk::surface_t(vk_instance, window);
    auto physical_device = pick_physical_device(vk_instance, surface);

    const auto queue_graphics_family_index {
        physical_device.queue_family_index(VK_QUEUE_GRAPHICS_BIT)
    };

    const auto queue_present_family_index {
        physical_device.queue_surface_support_index(surface)
    };

    const auto queue_family_indices = std::set<int> {
        queue_graphics_family_index,
        queue_present_family_index
    };

#ifndef NDEBUG
    auto logical_device = vk::logical_device_t(
                physical_device,
                required_device_extensions,
                queue_family_indices,
                validation_layers);
#else
    auto logical_device = vk::logical_device_t(
                physical_device,
                required_device_extensions,
                queue_family_indices);
#endif
    auto graphics_queue = vk::queue_t(logical_device,
                                      queue_graphics_family_index);
    auto present_queue = vk::queue_t(logical_device,
                                     queue_present_family_index);

    auto swap_chain = vk::swap_chain_t(
                logical_device,
                surface,
                physical_device.surface_capabilities(surface),
                choose_swap_surface_format(physical_device, surface),
                choose_swap_present_mode(physical_device, surface),
                choose_swap_extent(physical_device, surface, window),
                queue_family_indices);

    auto images = swap_chain.images();
    std::vector<std::unique_ptr<vk::image_view_t>> image_views;
    image_views.reserve(images.size());
    for (auto &image: images)
    {
        image_views.emplace_back(new vk::image_view_t(
                                     logical_device,
                                     image,
                                     VK_IMAGE_VIEW_TYPE_2D,
                                     swap_chain.format()));
    }

    auto render_pass = create_render_pass(logical_device, swap_chain);
    auto pipeline_layout = vk::pipeline_layout_t(logical_device);
    auto graphics_pipeline = create_graphics_pipeline(
                logical_device,
                swap_chain,
                render_pass,
                pipeline_layout);

    std::vector<std::unique_ptr<vk::framebuffer_t>> framebuffers;
    framebuffers.reserve(image_views.size());
    for (const auto &image_view: image_views)
    {
        framebuffers.emplace_back(new vk::framebuffer_t(
                                      logical_device,
                                      render_pass,
                                      swap_chain.extent().width,
                                      swap_chain.extent().height,
                                      { image_view.get() }));
    }

    auto command_pool = vk::command_pool_t(
                logical_device,
                queue_graphics_family_index);

    auto command_buffer = vk::command_buffer_t(
                logical_device,
                command_pool,
                framebuffers.size());

    for (std::size_t index = 0; index < command_buffer.size(); ++index)
    {
        const VkCommandBufferBeginInfo begin_info {
            VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            nullptr,
            VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
            nullptr
        };

        vkBeginCommandBuffer(command_buffer[index], &begin_info);

        const VkClearValue clear_color {
            0.0f, 0.0f, 0.0f, 1.0f
        };

        const VkRenderPassBeginInfo render_pass_info {
            VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            nullptr,
            render_pass,
            *framebuffers[index].get(),
            { { 0, 0 }, swap_chain.extent() },
            1,
            &clear_color
        };

        vkCmdBeginRenderPass(
                    command_buffer[index],
                    &render_pass_info,
                    VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(
                    command_buffer[index],
                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                    graphics_pipeline);

        vkCmdDraw(command_buffer[index], 3, 1, 0, 0);

        vkCmdEndRenderPass(command_buffer[index]);

        if (auto result = vkEndCommandBuffer(command_buffer[index]);
                result != VK_SUCCESS)
        {

            LOG_FATAL("Failed to create render pass. Error: %s",
                      vk::support::VK_RESULT_STRING.at(result));
        }
    }

    auto image_available = vk::semaphore_t { logical_device };
    auto render_finished = vk::semaphore_t { logical_device };

    window.runMainLoop(
                &draw_frame,
                logical_device,
                swap_chain,
                command_buffer,
                graphics_queue,
                present_queue,
                image_available,
                render_finished);

    logical_device.wait_for_idle();
}
