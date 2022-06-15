#include "function/render/rhi/vulkan/vulkan_pipeline.h"

namespace Eagle
{
	void VulkanPipeline::initialize(RenderPipelineInitInfo init_info)
	{
		m_render_resource = init_info.render_resource;
	}
}