#pragma once
#ifndef DRAKO_VULKAN_FORWARD_RENDERER_HPP
#define DRAKO_VULKAN_FORWARD_RENDERER_HPP

#include "drako/graphics/vulkan_material_pipeline.hpp"
#include "drako/graphics/vulkan_mesh_types.hpp"
#include "drako/graphics/vulkan_runtime_context.hpp"
#include "drako/math/mat4x4.hpp"

#include <vector>

#include <vulkan/vulkan.hpp>

namespace drako::vulkan
{
    class forward_renderer
    {
    public:
        explicit forward_renderer(
            const Context& ctx, std::uint32_t width, std::uint32_t height);

        ~forward_renderer() noexcept;

        forward_renderer(const forward_renderer&) = delete;
        forward_renderer& operator=(const forward_renderer&) = delete;

        forward_renderer(forward_renderer&&) = delete;
        forward_renderer& operator=(forward_renderer&&) = delete;

        [[nodiscard]] vk::RenderPass renderpass() const noexcept
        {
            return _renderpass.get();
        }

        struct draw_batch_soa
        {
            material_pipeline   pipeline;
            std::vector<Mat4x4> mvps;
            std::vector<mesh>   meshes;
        };
        void draw(const draw_batch_soa&) noexcept;

        struct draw_batch_mtl_soa
        {
            material_pipeline              pipeline;
            std::vector<Mat4x4>            mvps;
            std::vector<mesh>              meshes;
            std::vector<material_instance> materials;
        };
        void draw(const draw_batch_mtl_soa&) noexcept;

    private:
        struct frame_attachments
        {
            vk::Framebuffer framebuffer;
            // vk::Image       color_buffer; << provided by the swapchain
            vk::ImageView color_buffer_view;
            vk::Image     depth_buffer;
            vk::ImageView depth_buffer_view;
        };

        void _setup_descriptors() noexcept;

        void _setup_renderpass() noexcept;

        void _setup_swapchain(vk::PhysicalDevice pdevice) noexcept;

        const vk::PhysicalDevice _pdevice;
        const vk::Device         _ldevice;
        const vk::SurfaceKHR     _surface;

        vk::Rect2D             _render_area;
        vk::UniqueSwapchainKHR _swapchain;
        std::vector<vk::Image> _swapchain_images;

        vk::Queue     _command_queue;        // executes drawing commands
        vk::Queue     _present_queue;        // displays rendering results
        vk::Semaphore _command_complete_sem; // signaled when the rendering process completes
        vk::Semaphore _present_complete_sem; // signaled when the presentation process completes

        vk::UniqueRenderPass _renderpass;

        vk::CommandBuffer _command_buffer;

        std::vector<frame_attachments> _attachments;
    };

} // namespace drako::vulkan

#endif // !DRAKO_VULKAN_FORWARD_RENDERER_HPP