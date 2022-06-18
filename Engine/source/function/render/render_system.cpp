#include "function/render/render_system.h"

namespace Eagle
{
	void RenderSystem::initialize(RenderSystemInitInfo init_info)
	{
		RHIInitInfo rhi_init_info;
		rhi_init_info.window_system = init_info.window_system;
		m_rhi = std::make_shared<RHI>();
		m_rhi->initialize(rhi_init_info);

		m_render_resource = std::make_shared<RenderResource>();
		m_render_resource->initialize({ m_rhi });
		/*RenderMeshData mesh_data;
		mesh_data = m_render_resource->loadMeshData(mesh);
		RenderMaterialData material_data;
		material_data = m_render_resource->loadMaterialData(texture_file);
		m_render_resource->uploadMeshRenderResource(m_rhi, mesh_data, material_data);*/

		m_render_pipeline = std::make_shared<RenderPipeline>();
		RenderPipelineInitInfo pipeline_info{ m_rhi, m_render_resource };
		m_render_pipeline->initialize(pipeline_info);
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