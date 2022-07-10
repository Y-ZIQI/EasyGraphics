#pragma once

#include "function/render/rhi/vulkan/vulkan_pass.h"

namespace Eagle
{
	struct PostprocessPassInitInfo
	{
		std::shared_ptr<VulkanRHI> rhi;
		std::shared_ptr<VulkanPass> shading_pass_ptr;
	};

	class PostprocessPass : public VulkanPass
	{
	public:
		PostprocessPass() {};
		~PostprocessPass() {};

		void initialize(PostprocessPassInitInfo init_info);
		void draw();
		void cleanupSwapChain();
		void cleanup();

		void setupAttachments();
		void setupRenderPass();
		void setupDescriptorSetLayout();
		void setupPipelines();
		void setupFramebuffers();
		void setupUniformBuffers();
		void setupDescriptorSets();

		void updateRecreateSwapChain();

		VulkanFramebuffer* m_color_buffer_ptr;
	};

}