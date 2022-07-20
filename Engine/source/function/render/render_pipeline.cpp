#include "function/render/render_pipeline.h"

namespace Eagle
{
	void RenderPipeline::initialize(RenderPipelineInitInfo init_info)
	{
		m_rhi = init_info.rhi;
		m_render_resource = init_info.render_resource;

		m_directional_shadow_pass = std::make_shared<ShadowPass>();
		m_directional_shadow_pass->initialize({ init_info.rhi, init_info.render_resource, glm::uvec2(1024, 1024) });

		m_gbuffer_pass = std::make_shared<GBufferPass>();
		m_gbuffer_pass->initialize({ init_info.rhi, init_info.render_resource });

		m_shading_pass = std::make_shared<ShadingPass>();
		m_shading_pass->initialize({ init_info.rhi, init_info.render_resource, m_directional_shadow_pass, m_gbuffer_pass });

		m_postprocess_pass = std::make_shared<PostprocessPass>();
		m_postprocess_pass->initialize({ init_info.rhi, m_shading_pass });
	}

	bool RenderPipeline::render()
	{
		bool recreate_swapchain = m_rhi->preRendering();
		if (recreate_swapchain) {
			return true;
		}

		auto& m_scene = m_render_resource->m_current_scene;
		auto& m_camera = m_scene->m_cameras[0];
		for (auto& pair : m_render_resource->m_current_scene->m_material_meshes) {
			auto& mat_id = pair.first;
			auto& mesh_set = pair.second;
			for (auto& mesh_id : mesh_set) {
				m_render_resource->m_render_nodes[mat_id][mesh_id] = {
					m_render_resource->m_current_scene->m_transforms[mesh_id].transform,
					m_render_resource->m_current_scene->m_transforms[mesh_id].n_transform,
					mesh_id,
					mat_id
				};
			}
		}

		// Shadow Pass
		auto dir_light_proj_view = m_scene->m_dir_light.getViewProj(m_camera, 3.0f);
		m_directional_shadow_pass->m_per_frame_ubo.proj_view_matrix = dir_light_proj_view;
		m_directional_shadow_pass->draw();

		// GBuffer Pass
		m_gbuffer_pass->m_per_frame_ubo.proj_view_matrix = m_camera.getViewProj();
		m_gbuffer_pass->m_per_frame_ubo.camera_pos = m_camera.m_data.m_position;
		m_gbuffer_pass->draw();

		// Shading Pass
		m_shading_pass->m_per_frame_ubo.proj_view_matrix = m_camera.getViewProj();
		m_shading_pass->m_per_frame_ubo.camera_pos = m_camera.m_data.m_position;
		m_shading_pass->m_per_frame_ubo.dir_light.intensity = { m_scene->m_dir_light.intensity, m_scene->m_dir_light.ambient };
		m_shading_pass->m_per_frame_ubo.dir_light.direction = { m_scene->m_dir_light.direction, m_scene->m_dir_light.distance_to_camera };
		m_shading_pass->m_per_frame_ubo.dir_light.status = { (float)m_directional_shadow_pass->m_resolution.x, 0.0f, 0.0f, 0.0f };
		m_shading_pass->m_per_frame_ubo.dir_light.proj_view_matrix = dir_light_proj_view;
		m_shading_pass->draw();

		// Post process Pass
		m_postprocess_pass->draw();

		recreate_swapchain = m_rhi->postRendering();
		return recreate_swapchain;
	}

	void RenderPipeline::cleanup()
	{
		m_directional_shadow_pass->cleanup();
		m_gbuffer_pass->cleanup();
		m_shading_pass->cleanup();
		m_postprocess_pass->cleanup();
	}

	void RenderPipeline::recreateSwapChain()
	{
		m_rhi->recreateSwapChain();
		m_gbuffer_pass->updateRecreateSwapChain();
		m_shading_pass->updateRecreateSwapChain();
		m_postprocess_pass->updateRecreateSwapChain();
	}

	void RenderPipeline::reload()
	{
		m_directional_shadow_pass->reload();
		m_gbuffer_pass->reload();
		m_shading_pass->reload();
		m_postprocess_pass->reload();
	}
}