#include "function/render/render_pass.h"

namespace Eagle
{
	void RenderPass::initialize(RenderPassInitInfo init_info)
	{
		RHIRenderPass::initialize({ init_info.rhi, init_info.render_resource });
		m_render_resource = init_info.render_resource;
	}

	void RenderPass::updateUniforms()
	{
		auto& m_scene = m_render_resource->m_current_scene;
		m_vert_ubo.proj_view_matrix = m_scene->m_cameras[0].getViewProj();
	}

	void RenderPass::cleanup()
	{
		RHIRenderPass::cleanup();
	}
}