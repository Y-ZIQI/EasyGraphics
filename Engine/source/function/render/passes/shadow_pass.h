#pragma once

#include "function/render/rhi/vulkan/vulkan_pass.h"

namespace Eagle
{
	struct ShadowPassInitInfo
	{
		std::shared_ptr<VulkanRHI> rhi;
		std::shared_ptr<VulkanRenderResource> render_resource;
		glm::uvec2 resolution;
	};

	class ShadowPass : public VulkanPass
	{
	public:
		ShadowPass() {};
		~ShadowPass() {};

		void initialize(ShadowPassInitInfo init_info);
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

		void recreateShadpwMap();

		glm::uvec2 m_resolution{ 512, 512 };

		ShadowPerFrameUBO	m_per_frame_ubo;
		ShadowPerDrawUBO	m_per_draw_ubo;
	};

}