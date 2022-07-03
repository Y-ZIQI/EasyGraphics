#include "function/global/global_resource.h"

#include "function/render/rhi/vulkan/vulkan_pass.h"

namespace Eagle
{
	void VulkanPass::initialize(VulkanPassInitInfo init_info)
	{
		m_rhi = init_info.rhi;
		m_render_resource = init_info.render_resource;

		setupRenderPass();
        setupDescriptorSetLayout();
		setupPipelines();
        setupFramebuffers();
        setupUniformBuffers();
        setupDescriptorSets();
	}

    void VulkanPass::draw()
    {
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_render_pass;
        renderPassInfo.framebuffer = m_rhi->m_swapchain_framebuffers[m_rhi->m_current_swapchain_image_index];
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = m_rhi->m_swapchain_extent;

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
        clearValues[1].depthStencil = { 1.0f, 0 };
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        VkCommandBuffer& commandBuffer = m_rhi->m_command_buffers[m_rhi->m_current_frame_index];

        m_rhi->m_vk_cmd_begin_render_pass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        m_rhi->m_vk_cmd_bind_pipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_render_pipelines[0].pipeline);

        memcpy(m_uniform_buffers[0].memory_pointer[m_rhi->m_current_frame_index], &m_per_frame_ubo, sizeof(m_per_frame_ubo));

        m_rhi->m_vk_cmd_bind_descriptor_sets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_render_pipelines[0].layout, 0, 1, &m_descriptors[0].descriptor_set, 0, nullptr);

        for (auto& pair : m_render_resource->m_render_nodes) {
            auto& material = m_render_resource->m_render_materials[pair.first];
            auto& mesh_map = pair.second;

            m_rhi->m_vk_cmd_bind_descriptor_sets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_render_pipelines[0].layout, 2, 1, &material.material_descriptor_set, 0, nullptr);

            for (auto& node_pair : mesh_map) {
                VulkanMesh& render_mesh = m_render_resource->m_render_meshes[node_pair.first];
                VulkanMeshNode& render_node = node_pair.second;

                m_per_draw_ubo.model_matrix = render_node.model_matrix;
                memcpy(m_uniform_buffers[1].memory_pointer[m_rhi->m_current_frame_index], &m_per_draw_ubo, sizeof(m_per_draw_ubo));

                m_rhi->m_vk_cmd_bind_descriptor_sets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_render_pipelines[0].layout, 1, 1, &m_descriptors[1].descriptor_set, 0, nullptr);

                VkBuffer vertexBuffers[] = { render_mesh.mesh_vertex_buffer };
                VkDeviceSize offsets[] = { 0 };
                m_rhi->m_vk_cmd_bind_vertex_buffers(commandBuffer, 0, 1, vertexBuffers, offsets);
                m_rhi->m_vk_cmd_bind_index_buffer(commandBuffer, render_mesh.mesh_index_buffer, 0, VK_INDEX_TYPE_UINT32);

                m_rhi->m_vk_cmd_draw_indexed(commandBuffer, render_mesh.mesh_index_count, 1, 0, 0, 0);
            }
        }

        m_rhi->m_vk_cmd_end_render_pass(commandBuffer);
    }

    void VulkanPass::cleanupSwapChain()
    {
		for (auto& pipeline_base : m_render_pipelines) {
			vkDestroyPipeline(m_rhi->m_device, pipeline_base.pipeline, nullptr);
			vkDestroyPipelineLayout(m_rhi->m_device, pipeline_base.layout, nullptr);
		}
		vkDestroyRenderPass(m_rhi->m_device, m_render_pass, nullptr);
    }

	void VulkanPass::cleanup()
	{
        cleanupSwapChain();

        for (size_t i = 0; i < m_rhi->m_max_frames_in_flight; i++) {
            vkUnmapMemory(m_rhi->m_device, m_uniform_buffers[0].uniform_buffers_memory[i]);
            vkDestroyBuffer(m_rhi->m_device, m_uniform_buffers[0].uniform_buffers[i], nullptr);
            vkFreeMemory(m_rhi->m_device, m_uniform_buffers[0].uniform_buffers_memory[i], nullptr);

            vkUnmapMemory(m_rhi->m_device, m_uniform_buffers[1].uniform_buffers_memory[i]);
            vkDestroyBuffer(m_rhi->m_device, m_uniform_buffers[1].uniform_buffers[i], nullptr);
            vkFreeMemory(m_rhi->m_device, m_uniform_buffers[1].uniform_buffers_memory[i], nullptr);
        }
        vkDestroyDescriptorSetLayout(m_rhi->m_device, m_descriptors[0].layout, nullptr);
        vkDestroyDescriptorSetLayout(m_rhi->m_device, m_descriptors[1].layout, nullptr);
        vkDestroyDescriptorSetLayout(m_rhi->m_device, m_descriptors[2].layout, nullptr);
	}

	void VulkanPass::setupRenderPass()
	{
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = m_rhi->m_swapchain_image_format;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentDescription depthAttachment{};
        depthAttachment.format = m_rhi->m_depth_image_format;
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        if (vkCreateRenderPass(m_rhi->m_device, &renderPassInfo, nullptr, &m_render_pass) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render pass!");
        }
	}

    void VulkanPass::setupDescriptorSetLayout()
    {
        m_descriptors.resize(3);

        {
            // Global uniforms
            VkDescriptorSetLayoutBinding uboLayoutBinding{};
            uboLayoutBinding.binding = 0;
            uboLayoutBinding.descriptorCount = 1;
            uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            uboLayoutBinding.pImmutableSamplers = nullptr;
            uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

            VkDescriptorSetLayoutCreateInfo layoutInfo{};
            std::array<VkDescriptorSetLayoutBinding, 1> bindings = { uboLayoutBinding };
            layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
            layoutInfo.pBindings = bindings.data();

            if (vkCreateDescriptorSetLayout(m_rhi->m_device, &layoutInfo, nullptr, &m_descriptors[0].layout) != VK_SUCCESS) {
                throw std::runtime_error("failed to create descriptor set layout!");
            }
        }

        {
            // Per draw
            VkDescriptorSetLayoutBinding uboLayoutBinding{};
            uboLayoutBinding.binding = 0;
            uboLayoutBinding.descriptorCount = 1;
            uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            uboLayoutBinding.pImmutableSamplers = nullptr;
            uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

            VkDescriptorSetLayoutCreateInfo layoutInfo{};
            std::array<VkDescriptorSetLayoutBinding, 1> bindings = { uboLayoutBinding };
            layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
            layoutInfo.pBindings = bindings.data();

            if (vkCreateDescriptorSetLayout(m_rhi->m_device, &layoutInfo, nullptr, &m_descriptors[1].layout) != VK_SUCCESS) {
                throw std::runtime_error("failed to create descriptor set layout!");
            }
            // per draw, not per mesh, so this line is not used
            //m_render_resource->m_mesh_descriptor_set_layout = &m_descriptors[1].layout;
        }

        {
            // Per material
            // VkDescriptorSetLayoutBinding uboLayoutBinding{};
            // uboLayoutBinding.binding = 0;
            // uboLayoutBinding.descriptorCount = 1;
            // uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            // uboLayoutBinding.pImmutableSamplers = nullptr;
            // uboLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

            std::array<VkDescriptorSetLayoutBinding, 5> layoutBinding;

            layoutBinding[0].binding = 0;
            layoutBinding[0].descriptorCount = 1;
            layoutBinding[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            layoutBinding[0].pImmutableSamplers = nullptr;
            layoutBinding[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

            layoutBinding[1].binding = 1;
            layoutBinding[1].descriptorCount = 1;
            layoutBinding[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            layoutBinding[1].pImmutableSamplers = nullptr;
            layoutBinding[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

            layoutBinding[2].binding = 2;
            layoutBinding[2].descriptorCount = 1;
            layoutBinding[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            layoutBinding[2].pImmutableSamplers = nullptr;
            layoutBinding[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

            layoutBinding[3].binding = 3;
            layoutBinding[3].descriptorCount = 1;
            layoutBinding[3].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            layoutBinding[3].pImmutableSamplers = nullptr;
            layoutBinding[3].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

            layoutBinding[4].binding = 4;
            layoutBinding[4].descriptorCount = 1;
            layoutBinding[4].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            layoutBinding[4].pImmutableSamplers = nullptr;
            layoutBinding[4].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

            // VkDescriptorSetLayoutBinding samplerLayoutBinding{};
            // samplerLayoutBinding.binding = 1;
            // samplerLayoutBinding.descriptorCount = 1;
            // samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            // samplerLayoutBinding.pImmutableSamplers = nullptr;
            // samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

            VkDescriptorSetLayoutCreateInfo layoutInfo{};
            // std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };
            layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            layoutInfo.bindingCount = static_cast<uint32_t>(layoutBinding.size());
            layoutInfo.pBindings = layoutBinding.data();

            if (vkCreateDescriptorSetLayout(m_rhi->m_device, &layoutInfo, nullptr, &m_descriptors[2].layout) != VK_SUCCESS) {
                throw std::runtime_error("failed to create descriptor set layout!");
            }
            // DescriptorSetLayout pointers for render resource
            m_render_resource->m_material_descriptor_set_layout = &m_descriptors[2].layout;
        }
    }

	void VulkanPass::setupPipelines()
	{
        m_render_pipelines.resize(1);

        auto vertShaderCode = FileSystem::readFile(g_global_resource.m_shaders->vert_shader_path);
        auto fragShaderCode = FileSystem::readFile(g_global_resource.m_shaders->frag_shader_path);

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

        auto bindingDescription = VulkanVertex::getBindingDescription();
        auto attributeDescriptions = VulkanVertex::getAttributeDescriptions();

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

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
        //rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineDepthStencilStateCreateInfo depthStencil{};
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
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

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        std::vector<VkDescriptorSetLayout> buf = { m_descriptors[0].layout, m_descriptors[1].layout, m_descriptors[2].layout };
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(buf.size());
        pipelineLayoutInfo.pSetLayouts = buf.data();

        if (vkCreatePipelineLayout(m_rhi->m_device, &pipelineLayoutInfo, nullptr, &m_render_pipelines[0].layout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }

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

    void VulkanPass::setupFramebuffers()
    {
        m_rhi->m_swapchain_framebuffers.resize(m_rhi->m_swapchain_imageviews.size());

        for (size_t i = 0; i < m_rhi->m_swapchain_imageviews.size(); i++) {
            std::array<VkImageView, 2> attachments = {
                m_rhi->m_swapchain_imageviews[i],
                m_rhi->m_depth_image_view
            };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = m_render_pass;
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = m_rhi->m_swapchain_extent.width;
            framebufferInfo.height = m_rhi->m_swapchain_extent.height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(m_rhi->m_device, &framebufferInfo, nullptr, &m_rhi->m_swapchain_framebuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }
    }

    void VulkanPass::setupUniformBuffers()
    {
        m_uniform_buffers.resize(2);
        {
            VkDeviceSize bufferSize = sizeof(MeshPerFrameUBO);

            m_uniform_buffers[0].uniform_buffers.resize(m_rhi->m_max_frames_in_flight);
            m_uniform_buffers[0].uniform_buffers_memory.resize(m_rhi->m_max_frames_in_flight);
            m_uniform_buffers[0].memory_pointer.resize(m_rhi->m_max_frames_in_flight);

            for (size_t i = 0; i < m_rhi->m_max_frames_in_flight; i++) {
                VulkanUtil::createBuffer(
                    m_rhi->m_physical_device,
                    m_rhi->m_device,
                    bufferSize,
                    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                    m_uniform_buffers[0].uniform_buffers[i],
                    m_uniform_buffers[0].uniform_buffers_memory[i]
                );
                vkMapMemory(m_rhi->m_device, m_uniform_buffers[0].uniform_buffers_memory[i], 0, sizeof(MeshPerFrameUBO), 0, &m_uniform_buffers[0].memory_pointer[i]);
            }
        }

        {
            VkDeviceSize bufferSize = sizeof(MeshPerDrawUBO);

            m_uniform_buffers[1].uniform_buffers.resize(m_rhi->m_max_frames_in_flight);
            m_uniform_buffers[1].uniform_buffers_memory.resize(m_rhi->m_max_frames_in_flight);
            m_uniform_buffers[1].memory_pointer.resize(m_rhi->m_max_frames_in_flight);

            for (size_t i = 0; i < m_rhi->m_max_frames_in_flight; i++) {
                VulkanUtil::createBuffer(
                    m_rhi->m_physical_device,
                    m_rhi->m_device,
                    bufferSize,
                    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                    m_uniform_buffers[1].uniform_buffers[i],
                    m_uniform_buffers[1].uniform_buffers_memory[i]
                );
                vkMapMemory(m_rhi->m_device, m_uniform_buffers[1].uniform_buffers_memory[i], 0, sizeof(MeshPerDrawUBO), 0, &m_uniform_buffers[1].memory_pointer[i]);
            }
        }
    }

    void VulkanPass::setupDescriptorSets()
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

            for (size_t i = 0; i < m_rhi->m_max_frames_in_flight; i++) {
                VkDescriptorBufferInfo bufferInfo{};
                bufferInfo.buffer = m_uniform_buffers[0].uniform_buffers[i];
                bufferInfo.offset = 0;
                bufferInfo.range = sizeof(MeshPerFrameUBO);

                VkWriteDescriptorSet descriptorWrites{};
                descriptorWrites.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrites.dstSet = m_descriptors[0].descriptor_set;
                descriptorWrites.dstBinding = 0;
                descriptorWrites.dstArrayElement = 0;
                descriptorWrites.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                descriptorWrites.descriptorCount = 1;
                descriptorWrites.pBufferInfo = &bufferInfo;

                vkUpdateDescriptorSets(m_rhi->m_device, 1, &descriptorWrites, 0, nullptr);
            }
        }

        {
            VkDescriptorSetAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            allocInfo.descriptorPool = m_rhi->m_descriptor_pool;
            allocInfo.descriptorSetCount = 1;
            allocInfo.pSetLayouts = &m_descriptors[1].layout;

            if (vkAllocateDescriptorSets(m_rhi->m_device, &allocInfo, &m_descriptors[1].descriptor_set) != VK_SUCCESS) {
                throw std::runtime_error("failed to allocate descriptor sets!");
            }

            for (size_t i = 0; i < m_rhi->m_max_frames_in_flight; i++) {
                VkDescriptorBufferInfo bufferInfo{};
                bufferInfo.buffer = m_uniform_buffers[1].uniform_buffers[i];
                bufferInfo.offset = 0;
                bufferInfo.range = sizeof(MeshPerDrawUBO);

                VkWriteDescriptorSet descriptorWrites{};
                descriptorWrites.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrites.dstSet = m_descriptors[1].descriptor_set;
                descriptorWrites.dstBinding = 0;
                descriptorWrites.dstArrayElement = 0;
                descriptorWrites.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                descriptorWrites.descriptorCount = 1;
                descriptorWrites.pBufferInfo = &bufferInfo;

                vkUpdateDescriptorSets(m_rhi->m_device, 1, &descriptorWrites, 0, nullptr);
            }
        }
    }

    void VulkanPass::updateRecreateSwapChain()
    {
        cleanupSwapChain();

        setupRenderPass();
        setupPipelines();
        setupFramebuffers();
    }
}