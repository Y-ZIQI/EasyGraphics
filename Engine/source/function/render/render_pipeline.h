#pragma once

#include "function/render/rhi/rhi.h"
#include "function/render/render_resource.h"

namespace Eagle
{
	struct RenderPipelineInitInfo
	{
		std::shared_ptr<RHI> rhi;
		std::shared_ptr<RenderResource> render_resource;
	};

	class RenderPipeline
	{
	public:
		RenderPipeline() {};
		~RenderPipeline() {};

		void initialize(RenderPipelineInitInfo init_info);
		bool render();
		void cleanup();

		void recreateSwapChain();

		std::shared_ptr<RHI> m_rhi;

		//std::shared_ptr<RenderPass> m_main_pass;
		std::shared_ptr<GBufferPass> m_gbuffer_pass;
		std::shared_ptr<ShadingPass> m_shading_pass;

		std::shared_ptr<RenderResource> m_render_resource;
	};
}