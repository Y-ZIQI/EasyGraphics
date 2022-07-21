#pragma once

#include "function/render/rhi/vulkan/vulkan_pass.h"

namespace Eagle
{
	struct ShadingPassInitInfo
	{
		std::shared_ptr<VulkanRHI> rhi;
		std::shared_ptr<VulkanRenderResource> render_resource;

		std::vector<std::shared_ptr<VulkanPass>> shadow_pass_ptrs;
		std::shared_ptr<VulkanPass> gbuffer_pass_ptr;
	};

	class ShadingPass : public VulkanPass
	{
	public:
		ShadingPass() {};
		~ShadingPass() {};

		void initialize(ShadingPassInitInfo init_info);
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

		std::vector<VulkanFramebuffer*> m_shadow_map_ptrs;
		VulkanFramebuffer* m_gbuffer_ptr;

		ShadingPerFrameUBO	m_per_frame_ubo;
	};

}