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
		m_per_frame_ubo.proj_view_matrix = m_scene->m_cameras[0].getViewProj();
	}

	void RenderPass::render()
	{
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
		RHIRenderPass::draw();
	}

	void RenderPass::cleanup()
	{
		RHIRenderPass::cleanup();
	}
}