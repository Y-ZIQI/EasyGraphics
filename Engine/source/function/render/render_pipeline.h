#pragma once

#include "function/render/render_pass.h"

//#include "function/render/gbuffer_pass.h"

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

		std::shared_ptr<RenderPass> m_main_pass;
	};
}