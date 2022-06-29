#pragma once

#include "function/render/rhi/rhi.h"
#include "function/render/render_resource.h"

namespace Eagle
{
	struct RenderPassInitInfo
	{
		std::shared_ptr<RHI> rhi;
		std::shared_ptr<RenderResource> render_resource;
	};

	class RenderPass : public RHIRenderPass
	{
	public:
		RenderPass() {};
		~RenderPass() {};

		void initialize(RenderPassInitInfo init_info);
		void updateUniforms();
		void render();
		void cleanup();

		std::shared_ptr<RenderResource> m_render_resource;
	};
}