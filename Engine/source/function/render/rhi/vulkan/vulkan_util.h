#pragma once

#include "function/render/rhi/vulkan/vulkan_rhi.h"

#include <vulkan/vulkan.h>

#include <stdexcept>
#include <unordered_map>

namespace Eagle
{
	class VulkanUtil
	{
	public:
		static uint32_t findMemoryType(VkPhysicalDevice physical_device, uint32_t type_filter, VkMemoryPropertyFlags properties_flag);
		static VkShaderModule createShaderModule(VkDevice device, const std::vector<char>& shader_code);
		static void createBuffer(
			VkPhysicalDevice physical_device,
			VkDevice device,
			VkDeviceSize size, 
			VkBufferUsageFlags usage, 
			VkMemoryPropertyFlags properties, 
			VkBuffer& buffer, 
			VkDeviceMemory& bufferMemory
		);
		static void copyBuffer(
			VulkanRHI* rhi,
			VkBuffer srcBuffer, 
			VkBuffer dstBuffer, 
			VkDeviceSize size
		);
		static void createImage(
			VkPhysicalDevice physical_device,
			VkDevice device,
			uint32_t image_width,
			uint32_t image_height,
			VkFormat format,
			VkImageTiling image_tiling,
			VkImageUsageFlags image_usage_flags,
			VkMemoryPropertyFlags memory_property_flags,
			VkImage& image,
			VkDeviceMemory& memory,
			VkImageCreateFlags image_create_flags,
			uint32_t array_layers,
			uint32_t miplevels
		);
		static VkImageView createImageView(
			VkDevice device,
			VkImage& image,
			VkFormat format,
			VkImageAspectFlags image_aspect_flags,
			VkImageViewType view_type,
			uint32_t layout_count,
			uint32_t miplevels
		);
		static void transitionImageLayout(
			VulkanRHI* rhi,
			VkImage image,
			VkImageLayout old_layout,
			VkImageLayout new_layout,
			uint32_t layer_count,
			uint32_t miplevels,
			VkImageAspectFlags aspect_mask_bits
		);
		static void copyBufferToImage(
			VulkanRHI* rhi,
			VkBuffer buffer,
			VkImage  image,
			uint32_t width,
			uint32_t height,
			uint32_t layer_count
		);
		static VkSampler getMipmapSampler(VkPhysicalDevice physical_device, VkDevice device, uint32_t width, uint32_t height);
		static VkSampler getNearestSampler(VkPhysicalDevice physical_device, VkDevice device);
		static VkSampler getLinearSampler(VkPhysicalDevice physical_device, VkDevice device);
		static void      destroySamplers(VkDevice device);

	private:
		static std::unordered_map<uint32_t, VkSampler> m_mipmap_sampler_map;
		static VkSampler                               m_nearest_sampler;
		static VkSampler                               m_linear_sampler;
	};
}