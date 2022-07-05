#include "function/render/render_system.h"

namespace Eagle
{
	void RenderSystem::initialize(RenderSystemInitInfo init_info)
	{
		m_rhi = std::make_shared<RHI>();
		m_rhi->initialize({ init_info.window_system });

		m_render_resource = std::make_shared<RenderResource>();
		m_render_resource->initialize({ m_rhi });

		m_render_pipeline = std::make_shared<RenderPipeline>();
		m_render_pipeline->initialize({ m_rhi, m_render_resource });
	}

	void RenderSystem::tick()
	{
		if (m_render_pipeline->render())
			m_render_pipeline->recreateSwapChain();
	}

	void RenderSystem::cleanup()
	{
		m_render_resource->cleanup();
		m_render_pipeline->cleanup();
		m_rhi->cleanup();
	}
}