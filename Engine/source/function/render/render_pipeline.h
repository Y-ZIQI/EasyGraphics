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
		void reload();

		std::shared_ptr<RHI> m_rhi;

		// CSM passes
		std::vector<std::shared_ptr<ShadowPass>> m_directional_shadow_passes;

		std::shared_ptr<GBufferPass> m_gbuffer_pass;
		std::shared_ptr<ShadingPass> m_shading_pass;
		std::shared_ptr<PostprocessPass> m_postprocess_pass;

		std::shared_ptr<RenderResource> m_render_resource;
	};
}