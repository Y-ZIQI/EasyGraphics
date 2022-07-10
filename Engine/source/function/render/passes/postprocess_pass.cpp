#include "function/global/global_resource.h"

#include "function/render/passes/postprocess_pass.h"

namespace Eagle
{
	void PostprocessPass::initialize(PostprocessPassInitInfo init_info)
	{
		VulkanPass::initialize({ init_info.rhi , nullptr });
		m_color_buffer_ptr = &init_info.shading_pass_ptr->m_framebuffer;

		//setupAttachments();
		setupRenderPass();
		setupDescriptorSetLayout();
		setupPipelines();
		setupFramebuffers();
		setupUniformBuffers();
		setupDescriptorSets();
	}

	void PostprocessPass::draw()
	{
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_render_pass;
		renderPassInfo.framebuffer = m_rhi->m_swapchain_framebuffers[m_rhi->m_current_swapchain_image_index];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = m_rhi->m_swapchain_extent;

		std::array<VkClearValue, 1> clearValues;
		clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
		renderPassInfo.clearValueCount = clearValues.size();
		renderPassInfo.pClearValues = clearValues.data();

		VkCommandBuffer& commandBuffer = m_rhi->m_command_buffers[m_rhi->m_current_frame_index];

		m_rhi->m_vk_cmd_begin_render_pass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		m_rhi->m_vk_cmd_bind_pipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_render_pipelines[0].pipeline);
		m_rhi->m_vk_cmd_draw(commandBuffer, 3, 1, 0, 0);
		m_rhi->m_vk_cmd_end_render_pass(commandBuffer);
	}

	void PostprocessPass::cleanupSwapChain()
	{
		VulkanPass::cleanupSwapChain();
	}

	void PostprocessPass::cleanup()
	{
		VulkanPass::cleanup();
	}

	void PostprocessPass::setupAttachments()
	{
		m_framebuffer.width = m_rhi->m_swapchain_extent.width;
		m_framebuffer.height = m_rhi->m_swapchain_extent.height;
		m_framebuffer.attachments.resize(1);

		m_framebuffer.attachments[0].format = m_rhi->m_swapchain_image_format;

		VulkanUtil::createImage(m_rhi->m_physical_device,
			m_rhi->m_device,
			m_rhi->m_swapchain_extent.width,
			m_rhi->m_swapchain_extent.height,
			m_framebuffer.attachments[0].format,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT |
			VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			m_framebuffer.attachments[0].image,
			m_framebuffer.attachments[0].mem,
			0,
			1,
			1);

		m_framebuffer.attachments[0].view = VulkanUtil::createImageView(m_rhi->m_device,
			m_framebuffer.attachments[0].image,
			m_framebuffer.attachments[0].format,
			VK_IMAGE_ASPECT_COLOR_BIT,
			VK_IMAGE_VIEW_TYPE_2D,
			1,
			1);
	}

	void PostprocessPass::setupRenderPass()
	{
		std::array<VkAttachmentDescription, 1> attachment;

		VkAttachmentDescription& attachment_description = attachment[0];
		attachment_description.format = m_rhi->m_swapchain_image_format;
		attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;
		attachment_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachment_description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachment_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachment_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachment_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachment_description.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef;
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = attachment.size();
		renderPassInfo.pAttachments = attachment.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		if (vkCreateRenderPass(m_rhi->m_device, &renderPassInfo, nullptr, &m_render_pass) != VK_SUCCESS) {
			throw std::runtime_error("failed to create render pass!");
		}
	}

	void PostprocessPass::setupDescriptorSetLayout()
	{
		m_descriptors.resize(1);

		{
			std::array<VkDescriptorSetLayoutBinding, 1> layoutBinding;

			layoutBinding[0].binding = 0;
			layoutBinding[0].descriptorCount = 1;
			layoutBinding[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			layoutBinding[0].pImmutableSamplers = nullptr;
			layoutBinding[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

			VkDescriptorSetLayoutCreateInfo layoutInfo{};
			layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			layoutInfo.bindingCount = layoutBinding.size();
			layoutInfo.pBindings = layoutBinding.data();

			if (vkCreateDescriptorSetLayout(m_rhi->m_device, &layoutInfo, nullptr, &m_descriptors[0].layout) != VK_SUCCESS) {
				throw std::runtime_error("failed to create descriptor set layout!");
			}
		}
	}

	void PostprocessPass::setupPipelines()
	{
		m_render_pipelines.resize(1);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		std::vector<VkDescriptorSetLayout> layouts = { m_descriptors[0].layout };
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(layouts.size());
		pipelineLayoutInfo.pSetLayouts = layouts.data();

		if (vkCreatePipelineLayout(m_rhi->m_device, &pipelineLayoutInfo, nullptr, &m_render_pipelines[0].layout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}

		auto vertShaderCode = FileSystem::readFile(g_global_resource.m_shaders->tonemapping_vert_path);
		auto fragShaderCode = FileSystem::readFile(g_global_resource.m_shaders->tonemapping_frag_path);

		VkShaderModule vertShaderModule = VulkanUtil::createShaderModule(m_rhi->m_device, vertShaderCode);
		VkShaderModule fragShaderModule = VulkanUtil::createShaderModule(m_rhi->m_device, fragShaderCode);

		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vertShaderModule;
		vertShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragShaderModule;
		fragShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 0;
		vertexInputInfo.vertexAttributeDescriptionCount = 0;
		vertexInputInfo.pVertexBindingDescriptions = nullptr;
		vertexInputInfo.pVertexAttributeDescriptions = nullptr;

		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)m_rhi->m_swapchain_extent.width;
		viewport.height = (float)m_rhi->m_swapchain_extent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = m_rhi->m_swapchain_extent;

		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;

		VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;

		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		VkPipelineDepthStencilStateCreateInfo depthStencil{};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = VK_FALSE;
		depthStencil.depthWriteEnable = VK_FALSE;
		depthStencil.depthCompareOp = VK_COMPARE_OP_ALWAYS;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.stencilTestEnable = VK_FALSE;

		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;

		VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f;
		colorBlending.blendConstants[1] = 0.0f;
		colorBlending.blendConstants[2] = 0.0f;
		colorBlending.blendConstants[3] = 0.0f;

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = &depthStencil;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.layout = m_render_pipelines[0].layout;
		pipelineInfo.renderPass = m_render_pass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

		if (vkCreateGraphicsPipelines(m_rhi->m_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_render_pipelines[0].pipeline) != VK_SUCCESS) {
			throw std::runtime_error("failed to create graphics pipeline!");
		}

		vkDestroyShaderModule(m_rhi->m_device, fragShaderModule, nullptr);
		vkDestroyShaderModule(m_rhi->m_device, vertShaderModule, nullptr);
	}

	void PostprocessPass::setupFramebuffers()
	{
		m_rhi->m_swapchain_framebuffers.resize(m_rhi->m_swapchain_imageviews.size());

		for (size_t i = 0; i < m_rhi->m_swapchain_imageviews.size(); i++) {
			std::array<VkImageView, 1> attachments = {
				m_rhi->m_swapchain_imageviews[i]
			};

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = m_render_pass;
			framebufferInfo.attachmentCount = attachments.size();
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = m_rhi->m_swapchain_extent.width;
			framebufferInfo.height = m_rhi->m_swapchain_extent.height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(m_rhi->m_device, &framebufferInfo, nullptr, &m_rhi->m_swapchain_framebuffers[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to create framebuffer!");
			}
		}
	}

	void PostprocessPass::setupUniformBuffers()
	{
	}

	void PostprocessPass::setupDescriptorSets()
	{
		{
			VkDescriptorSetAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfo.descriptorPool = m_rhi->m_descriptor_pool;
			allocInfo.descriptorSetCount = 1;
			allocInfo.pSetLayouts = &m_descriptors[0].layout;

			if (vkAllocateDescriptorSets(m_rhi->m_device, &allocInfo, &m_descriptors[0].descriptor_set) != VK_SUCCESS) {
				throw std::runtime_error("failed to allocate descriptor sets!");
			}

			VkDescriptorImageInfo color_buffer_info = {};
			color_buffer_info.sampler = VulkanUtil::getNearestSampler(m_rhi->m_physical_device, m_rhi->m_device);
			color_buffer_info.imageView = m_color_buffer_ptr->attachments[0].view;
			color_buffer_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			VkWriteDescriptorSet shading_write_info[1];

			VkWriteDescriptorSet& color_buffer_write_info = shading_write_info[0];
			color_buffer_write_info.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			color_buffer_write_info.pNext = NULL;
			color_buffer_write_info.dstSet = m_descriptors[0].descriptor_set;
			color_buffer_write_info.dstBinding = 0;
			color_buffer_write_info.dstArrayElement = 0;
			color_buffer_write_info.descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
			color_buffer_write_info.descriptorCount = 1;
			color_buffer_write_info.pImageInfo = &color_buffer_info;

			vkUpdateDescriptorSets(m_rhi->m_device, 1, shading_write_info, 0, nullptr);
		}
	}

	void PostprocessPass::updateRecreateSwapChain()
	{
		cleanupSwapChain();

		setupRenderPass();
		setupPipelines();
		setupFramebuffers();
		setupDescriptorSets();
	}
}