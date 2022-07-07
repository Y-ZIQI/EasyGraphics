#include "function/render/render_pipeline.h"

namespace Eagle
{
	void RenderPipeline::initialize(RenderPipelineInitInfo init_info)
	{
		m_rhi = init_info.rhi;
		m_render_resource = init_info.render_resource;

		m_gbuffer_pass = std::make_shared<GBufferPass>();
		m_gbuffer_pass->initialize({ init_info.rhi, init_info.render_resource });

		m_shading_pass = std::make_shared<ShadingPass>();
		m_shading_pass->initialize({ init_info.rhi, init_info.render_resource, m_gbuffer_pass });
	}

	bool RenderPipeline::render()
	{
		bool recreate_swapchain = m_rhi->preRendering();
		if (recreate_swapchain) {
			return true;
		}
		// GBuffer Pass
		auto& m_scene = m_render_resource->m_current_scene;
		m_gbuffer_pass->m_per_frame_ubo.proj_view_matrix = m_scene->m_cameras[0].getViewProj();
		for (auto& pair : m_render_resource->m_current_scene->m_material_meshes) {
			auto& mat_id = pair.first;
			auto& mesh_set = pair.second;
			for (auto& mesh_id : mesh_set) {
				m_render_resource->m_render_nodes[mat_id][mesh_id] = {
					m_render_resource->m_current_scene->m_transforms[mesh_id].transform,
					mesh_id,
					mat_id
				};
			}
		}
		m_gbuffer_pass->draw();

		// Shading Pass
		m_shading_pass->m_per_frame_ubo.proj_view_matrix = m_scene->m_cameras[0].getViewProj();
		m_shading_pass->draw();
		recreate_swapchain = m_rhi->postRendering();
		return recreate_swapchain;
	}

	void RenderPipeline::cleanup()
	{
		m_gbuffer_pass->cleanup();
		m_shading_pass->cleanup();
	}

	void RenderPipeline::recreateSwapChain()
	{
		m_rhi->recreateSwapChain();
		m_gbuffer_pass->updateRecreateSwapChain();
		m_shading_pass->updateRecreateSwapChain();
	}
}