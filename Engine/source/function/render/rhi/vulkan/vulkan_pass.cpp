#include "function/render/rhi/vulkan/vulkan_pass.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Eagle
{
    const char* vert_shader_path = "../Engine/shaders/compiled/vert.spv";
    const char* frag_shader_path = "../Engine/shaders/compiled/frag.spv";

	void VulkanPass::initialize(RenderPassInitInfo init_info)
	{
		m_rhi = init_info.rhi;
		m_render_resource = init_info.render_resource;

		setupRenderPass();
        setupDescriptorSetLayout();
		setupPipelines();
        setupFramebuffers();
        setupUniformBuffers();
        setupDescriptorPool();
        setupDescriptorSets();
	}

    void VulkanPass::updateUniformBuffer()
    {
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        auto view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        auto proj = glm::perspective(glm::radians(45.0f), m_rhi->m_swapchain_extent.width / (float)m_rhi->m_swapchain_extent.height, 0.1f, 10.0f);
        proj[1][1] *= -1;
        MainPassVertUBO ubo{};
        ubo.proj_view_matrix = proj * view;
        ubo.model_matrix = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

        void* data;
        vkMapMemory(m_rhi->m_device, m_uniform_buffers[0].uniform_buffers_memory[m_rhi->m_current_frame_index], 0, sizeof(ubo), 0, &data);
        memcpy(data, &ubo, sizeof(ubo));
        vkUnmapMemory(m_rhi->m_device, m_uniform_buffers[0].uniform_buffers_memory[m_rhi->m_current_frame_index]);
    }

    void VulkanPass::draw()
    {
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_render_pass;
        renderPassInfo.framebuffer = m_rhi->m_swapchain_framebuffers[m_rhi->m_current_swapchain_image_index];
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = m_rhi->m_swapchain_extent;

        VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        VkCommandBuffer& commandBuffer = m_rhi->m_command_buffers[m_rhi->m_current_frame_index];

        m_rhi->m_vk_cmd_begin_render_pass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        m_rhi->m_vk_cmd_bind_pipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_render_pipelines[0].pipeline);

        updateUniformBuffer();

        for (auto& pair : m_render_resource->m_render_meshes) {
            VulkanMesh render_mesh = pair.second;

            VkBuffer vertexBuffers[] = { render_mesh.mesh_vertex_buffer };
            VkDeviceSize offsets[] = { 0 };
            m_rhi->m_vk_cmd_bind_vertex_buffers(commandBuffer, 0, 1, vertexBuffers, offsets);
            m_rhi->m_vk_cmd_bind_index_buffer(commandBuffer, render_mesh.mesh_index_buffer, 0, VK_INDEX_TYPE_UINT16);
            //m_rhi->m_vk_cmd_bind_index_buffer(commandBuffer, render_mesh.mesh_index_buffer, 0, VK_INDEX_TYPE_UINT32);

            m_rhi->m_vk_cmd_bind_descriptor_sets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_render_pipelines[0].layout, 0, 1, &m_descriptors[0].descriptor_sets[m_rhi->m_current_frame_index], 0, nullptr);

            m_rhi->m_vk_cmd_draw_indexed(commandBuffer, render_mesh.mesh_index_count, 1, 0, 0, 0);
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
            vkDestroyBuffer(m_rhi->m_device, m_uniform_buffers[0].uniform_buffers[i], nullptr);
            vkFreeMemory(m_rhi->m_device, m_uniform_buffers[0].uniform_buffers_memory[i], nullptr);
        }
        vkDestroyDescriptorPool(m_rhi->m_device, m_descriptors[0].descriptor_pool, nullptr);
        vkDestroyDescriptorSetLayout(m_rhi->m_device, m_descriptors[0].layout, nullptr);

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

        VkAttachmentReference colorAttachmentRef{};
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
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
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
        m_descriptors.resize(1);

        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.pImmutableSamplers = nullptr;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutBinding samplerLayoutBinding{};
        samplerLayoutBinding.binding = 1;
        samplerLayoutBinding.descriptorCount = 1;
        samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerLayoutBinding.pImmutableSamplers = nullptr;
        samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };
        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        layoutInfo.pBindings = bindings.data();

        if (vkCreateDescriptorSetLayout(m_rhi->m_device, &layoutInfo, nullptr, &m_descriptors[0].layout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

	void VulkanPass::setupPipelines()
	{
        m_render_pipelines.resize(1);

        auto vertShaderCode = FileSystem::readFile(vert_shader_path);
        auto fragShaderCode = FileSystem::readFile(frag_shader_path);

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

        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(VulkanVertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(VulkanVertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(VulkanVertex, color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(VulkanVertex, texCoord);

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
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &m_descriptors[0].layout;

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
            VkImageView attachments[] = {
                m_rhi->m_swapchain_imageviews[i]
            };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = m_render_pass;
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = attachments;
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
        m_uniform_buffers.resize(1);
        
        VkDeviceSize bufferSize = sizeof(MainPassVertUBO);

        m_uniform_buffers[0].uniform_buffers.resize(m_rhi->m_max_frames_in_flight);
        m_uniform_buffers[0].uniform_buffers_memory.resize(m_rhi->m_max_frames_in_flight);

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
        }
    }

    void VulkanPass::setupDescriptorPool()
    {
        VkDescriptorPoolSize poolSize{};
        poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSize.descriptorCount = static_cast<uint32_t>(m_rhi->m_max_frames_in_flight);

        std::array<VkDescriptorPoolSize, 2> poolSizes{};
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(m_rhi->m_max_frames_in_flight);
        poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[1].descriptorCount = static_cast<uint32_t>(m_rhi->m_max_frames_in_flight);

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(m_rhi->m_max_frames_in_flight);

        if (vkCreateDescriptorPool(m_rhi->m_device, &poolInfo, nullptr, &m_descriptors[0].descriptor_pool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    void VulkanPass::setupDescriptorSets()
    {
        std::vector<VkDescriptorSetLayout> layouts(m_rhi->m_max_frames_in_flight, m_descriptors[0].layout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = m_descriptors[0].descriptor_pool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(m_rhi->m_max_frames_in_flight);
        allocInfo.pSetLayouts = layouts.data();

        m_descriptors[0].descriptor_sets.resize(m_rhi->m_max_frames_in_flight);
        if (vkAllocateDescriptorSets(m_rhi->m_device, &allocInfo, m_descriptors[0].descriptor_sets.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        for (size_t i = 0; i < m_rhi->m_max_frames_in_flight; i++) {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = m_uniform_buffers[0].uniform_buffers[i];
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(MainPassVertUBO);

            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;            
            imageInfo.imageView = m_render_resource->m_render_materials[0].base_color_image_view; // Should modified
            //imageInfo.sampler = VulkanUtil::getLinearSampler(m_rhi->m_physical_device, m_rhi->m_device);
            imageInfo.sampler = VulkanUtil::getMipmapSampler(
                m_rhi->m_physical_device, 
                m_rhi->m_device,
                m_render_resource->m_render_materials[0].desc.width,
                m_render_resource->m_render_materials[0].desc.height
            );

            std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = m_descriptors[0].descriptor_sets[i];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo = &bufferInfo;

            descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[1].dstSet = m_descriptors[0].descriptor_sets[i];
            descriptorWrites[1].dstBinding = 1;
            descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[1].descriptorCount = 1;
            descriptorWrites[1].pImageInfo = &imageInfo;

            vkUpdateDescriptorSets(m_rhi->m_device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
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