#pragma once

#include "function/render/rhi/vulkan/vulkan_pass.h"

namespace Eagle
{
	struct GBufferPassInitInfo
	{
		std::shared_ptr<VulkanRHI> rhi;
		std::shared_ptr<VulkanRenderResource> render_resource;
	};

	class GBufferPass : public VulkanPass
	{
	public:
		GBufferPass() {};
		~GBufferPass() {};

		void initialize(GBufferPassInitInfo init_info);
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

		MeshPerFrameUBO	m_per_frame_ubo;
		MeshPerDrawUBO	m_per_draw_ubo;
	};

}