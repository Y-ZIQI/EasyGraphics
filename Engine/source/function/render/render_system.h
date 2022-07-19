#pragma once

#include "function/render/rhi/rhi.h"
#include "function/render/render_resource.h"
#include "function/render/render_pipeline.h"

namespace Eagle
{
	class WindowSystem;

	struct RenderSystemInitInfo
	{
		std::shared_ptr<WindowSystem> window_system;
	};

	class RenderSystem
	{
	public:
		RenderSystem() {};
		~RenderSystem() {};

		void initialize(RenderSystemInitInfo init_info);
		void tick();
		void cleanup();

	//private:
		std::shared_ptr<RHI> m_rhi;

		std::shared_ptr<RenderResource> m_render_resource;
		std::shared_ptr<RenderPipeline> m_render_pipeline;

		bool m_reload = false;
	};
}