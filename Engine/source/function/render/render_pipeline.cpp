#include "function/render/render_pipeline.h"

namespace Eagle
{
	void RenderPipeline::initialize(RenderPipelineInitInfo init_info)
	{
		m_rhi = init_info.rhi;

		m_main_pass = std::make_shared<RenderPass>();
		m_main_pass->initialize({ init_info.rhi, init_info.render_resource });
	}

	bool RenderPipeline::render()
	{
		bool recreate_swapchain = m_rhi->preRendering();
		if (recreate_swapchain) {
			return true;
		}
		m_main_pass->updateUniforms();
		m_main_pass->render();
		recreate_swapchain = m_rhi->postRendering();
		return recreate_swapchain;
	}

	void RenderPipeline::cleanup()
	{
		m_main_pass->cleanup();
	}

	void RenderPipeline::recreateSwapChain()
	{
		m_rhi->recreateSwapChain();
		m_main_pass->updateRecreateSwapChain();
	}
}