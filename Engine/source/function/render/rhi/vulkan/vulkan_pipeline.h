#pragma once

#include "function/render/rhi/vulkan/vulkan_rhi.h"
#include "function/render/render_resource.h"

namespace Eagle
{
	struct RenderPipelineInitInfo
	{
		std::shared_ptr<RenderResource> render_resource;
	};

	class VulkanPipeline
	{
	public:
		VulkanPipeline() {};
		~VulkanPipeline() {};

		void initialize(RenderPipelineInitInfo init_info);


		std::shared_ptr<VulkanRHI> m_rhi;


		std::shared_ptr<RenderResource> m_render_resource;
	};
}