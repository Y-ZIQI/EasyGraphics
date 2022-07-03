#pragma once

#include "function/render/rhi/vulkan/vulkan_rhi.h"
#include "function/render/rhi/vulkan/vulkan_resource.h"

namespace Eagle
{
	struct GBufferPassInitInfo
	{
		std::shared_ptr<VulkanRHI> rhi;
		std::shared_ptr<VulkanRenderResource> render_resource;
	};

	class GBufferPass
	{
	public:
		GBufferPass() {};
		~GBufferPass() {};

		void initialize(GBufferPassInitInfo init_info);
		void updateUniformBuffer();
		void draw();
		void cleanupSwapChain();
		void cleanup();

		void setupRenderPass();
		void setupDescriptorSetLayout();
		void setupPipelines();
		void setupFramebuffers();
		void setupUniformBuffers();
		void setupDescriptorSets();

		void updateRecreateSwapChain();

		struct VulkanPipelineBase
		{
			VkPipelineLayout layout;
			VkPipeline       pipeline;
		};

		struct VulkanDescriptor
		{
			VkDescriptorSetLayout	layout;
			VkDescriptorSet			descriptor_set;
		};

		struct VulkanUniformBuffer
		{
			std::vector<VkBuffer>		uniform_buffers;
			std::vector<VkDeviceMemory> uniform_buffers_memory;
			std::vector<void*>			memory_pointer;
		};

		struct VulkanAttachment
		{
			VkImage        image;
			VkDeviceMemory mem;
			VkImageView    view;
			VkFormat       format;
		};

		struct VulkanFramebuffer
		{
			int           width;
			int           height;
			VkFramebuffer framebuffer;

			std::vector<VulkanAttachment> attachments;
		};

		VkRenderPass					m_render_pass;
		std::vector<VulkanPipelineBase>	m_render_pipelines;
		std::vector<VulkanDescriptor>	m_descriptors;
		VulkanFramebuffer               m_framebuffer;

		MeshPerFrameUBO					m_per_frame_ubo;
		MeshPerDrawUBO					m_per_draw_ubo;
		std::vector<VulkanUniformBuffer> m_uniform_buffers;

		std::shared_ptr<VulkanRHI>				m_rhi;
		std::shared_ptr<VulkanRenderResource>	m_render_resource;
	};

}