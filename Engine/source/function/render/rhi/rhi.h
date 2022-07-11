#pragma once

#define USE_VULKAN_RHI

#ifdef USE_VULKAN_RHI

#include "function/render/rhi/vulkan/vulkan_rhi.h"
#include "function/render/rhi/vulkan/vulkan_resource.h"

#include "function/render/passes/gbuffer_pass.h"
#include "function/render/passes/shading_pass.h"
#include "function/render/passes/postprocess_pass.h"

namespace Eagle
{
	typedef VulkanRHI RHI;
	typedef VulkanRenderResource RHIRenderResource;

	//class RHI : public VulkanRHI{
	//public:
	//	RHI() {}
	//	~RHI() {}
	//};
}

#endif // USE_VULKAN_RHI

