#include "function/global/global_resource.h"

#include "function/render/render_pipeline.h"

namespace Eagle
{
	void RenderPipeline::initialize(RenderPipelineInitInfo init_info)
	{
		m_rhi = init_info.rhi;
		m_render_resource = init_info.render_resource;

		std::vector<std::shared_ptr<RHIPass>> pass_ptr(g_global_setting.CSM_maps);
		m_directional_shadow_passes.resize(g_global_setting.CSM_maps);
		for (int i = 0; i < g_global_setting.CSM_maps; i++) {
			m_directional_shadow_passes[i] = std::make_shared<ShadowPass>();
			m_directional_shadow_passes[i]->initialize({ init_info.rhi, init_info.render_resource, glm::uvec2(1024, 1024) });
			pass_ptr[i] = m_directional_shadow_passes[i];
		}

		m_gbuffer_pass = std::make_shared<GBufferPass>();
		m_gbuffer_pass->initialize({ init_info.rhi, init_info.render_resource });

		m_shading_pass = std::make_shared<ShadingPass>();
		m_shading_pass->initialize({ init_info.rhi, init_info.render_resource, pass_ptr, m_gbuffer_pass });

		m_blur_pass = std::make_shared<ScreenPass>();
		m_blur_pass->initialize({
			init_info.rhi ,
			{init_info.rhi->m_swapchain_extent.width, init_info.rhi->m_swapchain_extent.height},
			g_global_resource.m_shaders->blur_vert_path,
			g_global_resource.m_shaders->blur_frag_path
		});
		m_blur_pass->addSourceTexture(&m_shading_pass->m_framebuffer.attachments[0]);
		m_blur_pass->addOutputTarget();
		m_blur_pass->setup();

		m_postprocess_pass = std::make_shared<PostprocessPass>();
		//m_postprocess_pass->initialize({ init_info.rhi, m_shading_pass });
		m_postprocess_pass->initialize({ init_info.rhi, m_blur_pass });
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
		std::vector<glm::mat4> dir_light_proj_views(g_global_setting.CSM_maps);
		for (int i = 0; i < g_global_setting.CSM_maps; i++) {
			dir_light_proj_views[i] = m_scene->m_dir_light.getViewProj(m_camera, g_global_setting.CSM_distances[i], g_global_setting.CSM_distances[i + 1]);
			m_directional_shadow_passes[i]->m_per_frame_ubo.proj_view_matrix = dir_light_proj_views[i];
			m_directional_shadow_passes[i]->draw();
		}

		// GBuffer Pass
		m_gbuffer_pass->m_per_frame_ubo.proj_view_matrix = m_camera.getViewProj();
		m_gbuffer_pass->m_per_frame_ubo.camera_pos = m_camera.m_data.m_position;
		m_gbuffer_pass->draw();

		// Shading Pass
		m_shading_pass->m_per_frame_ubo.proj_view_matrix = m_camera.getViewProj();
		m_shading_pass->m_per_frame_ubo.camera_pos = m_camera.m_data.m_position;
		m_shading_pass->m_per_frame_ubo.camera_params = { m_camera.m_data.m_aspect, m_camera.m_data.m_z_near, m_camera.m_data.m_z_far };
		m_shading_pass->m_per_frame_ubo.dir_light.intensity = { m_scene->m_dir_light.intensity, m_scene->m_dir_light.ambient };
		m_shading_pass->m_per_frame_ubo.dir_light.direction = { m_scene->m_dir_light.direction, m_scene->m_dir_light.distance_to_camera };
		m_shading_pass->m_per_frame_ubo.dir_light.status = { (float)m_directional_shadow_passes[0]->m_resolution.x, g_global_setting.CSM_distances[1], g_global_setting.CSM_distances[2], g_global_setting.CSM_distances[3] };
		m_shading_pass->m_per_frame_ubo.dir_light.proj_view_matrix_1 = dir_light_proj_views[0];
		m_shading_pass->m_per_frame_ubo.dir_light.proj_view_matrix_2 = dir_light_proj_views[1];
		m_shading_pass->m_per_frame_ubo.dir_light.proj_view_matrix_3 = dir_light_proj_views[2];
		m_shading_pass->draw();

		// Blur Pass
		m_blur_pass->draw();

		// Post process Pass
		m_postprocess_pass->draw();

		recreate_swapchain = m_rhi->postRendering();
		return recreate_swapchain;
	}

	void RenderPipeline::cleanup()
	{
		for (int i = 0; i < g_global_setting.CSM_maps; i++) {
			m_directional_shadow_passes[i]->cleanup();
		}
		m_directional_shadow_passes.clear();
		m_gbuffer_pass->cleanup();
		m_shading_pass->cleanup();
		m_blur_pass->cleanup();
		m_postprocess_pass->cleanup();
	}

	void RenderPipeline::recreateSwapChain()
	{
		m_rhi->recreateSwapChain();
		m_gbuffer_pass->updateRecreateSwapChain();
		m_shading_pass->updateRecreateSwapChain();
		m_blur_pass->updateRecreateSwapChain();
		m_postprocess_pass->updateRecreateSwapChain();
	}

	void RenderPipeline::reload()
	{
		for (int i = 0; i < g_global_setting.CSM_maps; i++) {
			m_directional_shadow_passes[i]->reload();
		}
		m_gbuffer_pass->reload();
		m_shading_pass->reload();
		m_blur_pass->reload();
		m_postprocess_pass->reload();
	}
}