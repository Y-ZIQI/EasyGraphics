#pragma once

#include "function/render/rhi/vulkan/vulkan_pass.h"

namespace Eagle
{
	struct ScreenPassInitInfo
	{
		std::shared_ptr<VulkanRHI> rhi;

		glm::uvec2 out_resolution;
		const char* vert_path;
		const char* frag_path;

		//std::shared_ptr<VulkanPass> src_pass_ptr;
		//VkFormat out_format{ VK_FORMAT_R8G8B8A8_SRGB };
	};

	class ScreenPass : public VulkanPass
	{
	public:
		ScreenPass() {};
		~ScreenPass() {};

		void initialize(ScreenPassInitInfo init_info);
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

		void setup();
		void addSourceTexture(VulkanAttachment* src_texture_ptr);
		void addOutputTarget(VkFormat out_format = VK_FORMAT_R8G8B8A8_UNORM);

		std::vector<VulkanAttachment*> m_src_texture_ptrs;
		glm::uvec2 m_resolution{ 512, 512 };
		const char* m_vert_path;
		const char* m_frag_path;
	};

}