#pragma once

#include "function/render/window_system.h"
#include "platform/file_system/file_system.h"

#define GLFW_INCLUDE_VULKAN
#include <glfw/glfw3.h>

#include <stdexcept>
#include <algorithm>
#include <vector>
#include <string>
#include <optional>
#include <set>

namespace Eagle
{
    struct RHIInitInfo
    {
        std::shared_ptr<WindowSystem> window_system;
    };

    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete() {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

	class VulkanRHI {
	public:
		VulkanRHI();

		void initialize(RHIInitInfo init_info);
        void cleanup();

    public:
        GLFWwindow*         m_window;
        VkInstance          m_instance;
        VkSurfaceKHR        m_surface;
        VkPhysicalDevice    m_physical_device = VK_NULL_HANDLE;
        VkDevice            m_device = VK_NULL_HANDLE;
        VkFormat            m_depth_image_format = VK_FORMAT_UNDEFINED;
        VkQueue             m_graphics_queue;
        VkQueue             m_present_queue;
        VkDebugUtilsMessengerEXT m_debug_messenger;

        VkSwapchainKHR              m_swapchain;
        std::vector<VkImage>        m_swapchain_images;
        VkFormat                    m_swapchain_image_format;
        VkExtent2D                  m_swapchain_extent;
        std::vector<VkImageView>    m_swapchain_imageviews;
        std::vector<VkFramebuffer>  m_swapchain_framebuffers;
        uint32_t    m_current_swapchain_image_index;

        VkImage        m_depth_image = VK_NULL_HANDLE;
        VkDeviceMemory m_depth_image_memory = VK_NULL_HANDLE;
        VkImageView    m_depth_image_view = VK_NULL_HANDLE;

        static uint8_t const            m_max_frames_in_flight;
        VkCommandPool                   m_command_pool;
        std::vector<VkCommandBuffer>    m_command_buffers;

        VkDescriptorPool m_descriptor_pool;
        // used in descriptor pool creation
        uint32_t m_max_vertex_blending_mesh_count = 256;
        uint32_t m_max_material_count = 256;

        std::vector<VkSemaphore>    m_image_available_semaphores;
        std::vector<VkSemaphore>    m_render_finished_semaphores;
        std::vector<VkFence>        m_in_flight_fences;
        uint32_t    m_current_frame_index = 0;

        // function pointers
        PFN_vkWaitForFences         m_vk_wait_for_fences;
        PFN_vkResetFences           m_vk_reset_fences;
        PFN_vkResetCommandPool      m_vk_reset_command_pool;
        PFN_vkBeginCommandBuffer    m_vk_begin_command_buffer;
        PFN_vkEndCommandBuffer      m_vk_end_command_buffer;
        PFN_vkCmdBeginRenderPass    m_vk_cmd_begin_render_pass;
        PFN_vkCmdNextSubpass        m_vk_cmd_next_subpass;
        PFN_vkCmdEndRenderPass      m_vk_cmd_end_render_pass;
        PFN_vkCmdBindPipeline       m_vk_cmd_bind_pipeline;
        PFN_vkCmdSetViewport        m_vk_cmd_set_viewport;
        PFN_vkCmdSetScissor         m_vk_cmd_set_scissor;
        PFN_vkCmdBindVertexBuffers  m_vk_cmd_bind_vertex_buffers;
        PFN_vkCmdBindIndexBuffer    m_vk_cmd_bind_index_buffer;
        PFN_vkCmdBindDescriptorSets m_vk_cmd_bind_descriptor_sets;
        PFN_vkCmdDrawIndexed        m_vk_cmd_draw_indexed;
        PFN_vkCmdDraw               m_vk_cmd_draw;
        PFN_vkCmdClearAttachments   m_vk_cmd_clear_attachments;

        bool m_framebuffer_resized = false;

        static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
            auto app = reinterpret_cast<VulkanRHI*>(glfwGetWindowUserPointer(window));
            app->m_framebuffer_resized = true;
        }
        
        // command
        VkCommandBuffer beginSingleTimeCommands();
        void endSingleTimeCommands(VkCommandBuffer command_buffer);

        // swapchain
        void cleanupSwapChain();
        void recreateSwapChain();

        void createInstance();
        void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
        void setupDebugMessenger();
        void createSurface();
        void pickPhysicalDevice();
        void createLogicalDevice();
        void createSwapChain();
        void createImageViews();
        void createCommandPool();
        void createCommandBuffers();
        void createDescriptorPool();
        void createSyncObjects();

        bool preRendering();
        bool postRendering();

        // createImage & createImageView & findMemoryType are method used within vulkan.h/cpp, outside use VulkanUtil::* replaced
        void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
        VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        
        void createDepthResources();
        VkFormat findDepthFormat();
        VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
        bool hasStencilComponent(VkFormat format);

        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
        bool isDeviceSuitable(VkPhysicalDevice device);
        bool checkDeviceExtensionSupport(VkPhysicalDevice device);
        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
        std::vector<const char*> getRequiredExtensions();
        bool checkValidationLayerSupport();

        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
            //std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

            return VK_FALSE;
        }
	};
}