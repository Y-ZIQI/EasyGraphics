#include "function/render/rhi/vulkan/vulkan_resource.h"

namespace Eagle
{
    void VulkanRenderResource::initialize(RenderResourceInitInfo init_info)
    {
        m_rhi = init_info.rhi;
    }

    VulkanMesh VulkanRenderResource::createMesh(RenderMeshData mesh_data)
    {
        VulkanMesh temp;

        uint32_t index_buffer_size = static_cast<uint32_t>(mesh_data.m_static_mesh_data.m_index_buffer->m_size);
        void* index_buffer_data = mesh_data.m_static_mesh_data.m_index_buffer->m_data;
        temp.mesh_index_count = index_buffer_size / sizeof(MeshIndexDataDefinition);

        uint32_t vertex_buffer_size = static_cast<uint32_t>(mesh_data.m_static_mesh_data.m_vertex_buffer->m_size);
        MeshVertexDataDefinition* vertex_buffer_data =
            reinterpret_cast<MeshVertexDataDefinition*>(mesh_data.m_static_mesh_data.m_vertex_buffer->m_data);
        temp.mesh_vertex_count = vertex_buffer_size / sizeof(MeshVertexDataDefinition);

        createVertexBuffer(vertex_buffer_data, vertex_buffer_size, temp);
        createIndexBuffer(index_buffer_data, index_buffer_size, temp);

        return temp;
    }

    bool VulkanRenderResource::createTexture(std::shared_ptr<TextureData> tex_data, VulkanTexture& n_texture)
    {
        if (!tex_data) {
            n_texture.width = 0;
            n_texture.height = 0;
            n_texture.mip_levels = 0;
            n_texture.image_view = NULL;
            return false;
        }

        n_texture.width = tex_data->m_width;
        n_texture.height = tex_data->m_height;
        n_texture.mip_levels = std::floor(std::log2(std::max(n_texture.width, n_texture.height))) + 1;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        VkDeviceSize imageSize = n_texture.width * n_texture.height * 4;

        VulkanUtil::createBuffer(
            m_rhi->m_physical_device,
            m_rhi->m_device,
            imageSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer,
            stagingBufferMemory
        );

        void* data;
        vkMapMemory(m_rhi->m_device, stagingBufferMemory, 0, imageSize, 0, &data);
        memcpy(data, tex_data->m_pixels, static_cast<size_t>(imageSize));
        vkUnmapMemory(m_rhi->m_device, stagingBufferMemory);

        VulkanUtil::createImage(
            m_rhi->m_physical_device,
            m_rhi->m_device,
            n_texture.width,
            n_texture.height,
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            n_texture.image,
            n_texture.image_memory,
            0,
            1,
            n_texture.mip_levels
        );

        VulkanUtil::transitionImageLayout(
            m_rhi.get(),
            n_texture.image,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            n_texture.mip_levels,
            VK_IMAGE_ASPECT_COLOR_BIT
        );
        VulkanUtil::copyBufferToImage(
            m_rhi.get(),
            stagingBuffer,
            n_texture.image,
            n_texture.width,
            n_texture.height,
            1
        );
        VulkanUtil::transitionImageLayout(
            m_rhi.get(),
            n_texture.image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            1,
            n_texture.mip_levels,
            VK_IMAGE_ASPECT_COLOR_BIT
        );

        vkDestroyBuffer(m_rhi->m_device, stagingBuffer, nullptr);
        vkFreeMemory(m_rhi->m_device, stagingBufferMemory, nullptr);

        VulkanUtil::generateMipmaps(m_rhi.get(), n_texture.image, VK_FORMAT_R8G8B8A8_SRGB, n_texture.width, n_texture.height, 1, n_texture.mip_levels);

        n_texture.image_view = VulkanUtil::createImageView(
            m_rhi->m_device,
            n_texture.image,
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_ASPECT_COLOR_BIT,
            VK_IMAGE_VIEW_TYPE_2D,
            1,
            n_texture.mip_levels
        );
        return true;
    }

    VulkanPBRMaterial VulkanRenderResource::createPBRMaterial(RenderMaterialData material_data)
    {
        VulkanPBRMaterial temp;
        MeshPerMaterialUBO material_uniforms;

        if (createTexture(material_data.m_base_color_texture, temp.base_color_texture)) {
            material_uniforms.flags |= (uint32_t)EAGLE_MATERIAL_FLAGS::EAGLE_MATERIAL_FLAGS_BASECOLOR;
        }
        if (createTexture(material_data.m_specular_texture, temp.specular_texture)) {
            material_uniforms.flags |= (uint32_t)EAGLE_MATERIAL_FLAGS::EAGLE_MATERIAL_FLAGS_SPECULAR;
        }
        if (createTexture(material_data.m_normal_texture, temp.normal_texture)) {
            material_uniforms.flags |= (uint32_t)EAGLE_MATERIAL_FLAGS::EAGLE_MATERIAL_FLAGS_NORMAL;
        }
        if (createTexture(material_data.m_emissive_texture, temp.emissive_texture)) {
            material_uniforms.flags |= (uint32_t)EAGLE_MATERIAL_FLAGS::EAGLE_MATERIAL_FLAGS_EMISSIVE;
        }

        // TODO: initialize material uniform buffer
        VkDeviceSize bufferSize = sizeof(MeshPerMaterialUBO);
        VulkanUtil::createBuffer(
            m_rhi->m_physical_device,
            m_rhi->m_device,
            bufferSize,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            temp.material_uniform_buffer,
            temp.material_uniform_buffer_memory
        );
        vkMapMemory(m_rhi->m_device, temp.material_uniform_buffer_memory, 0, sizeof(MeshPerMaterialUBO), 0, &temp.material_uniform_memory_pointer);
        memcpy(temp.material_uniform_memory_pointer, &material_uniforms, sizeof(material_uniforms));

        VkDescriptorSetAllocateInfo material_descriptor_set_alloc_info;
        material_descriptor_set_alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        material_descriptor_set_alloc_info.pNext = NULL;
        material_descriptor_set_alloc_info.descriptorPool = m_rhi->m_descriptor_pool;
        material_descriptor_set_alloc_info.descriptorSetCount = 1;
        material_descriptor_set_alloc_info.pSetLayouts = m_material_descriptor_set_layout;

        if (VK_SUCCESS != vkAllocateDescriptorSets(m_rhi->m_device, &material_descriptor_set_alloc_info, &temp.material_descriptor_set))
        {
            throw std::runtime_error("allocate material descriptor set");
        }

        VkDescriptorBufferInfo material_uniform_buffer_info = {};
        material_uniform_buffer_info.offset = 0;
        material_uniform_buffer_info.range = sizeof(MeshPerMaterialUBO);
        material_uniform_buffer_info.buffer = temp.material_uniform_buffer;

        VkDescriptorImageInfo image_info[4];
        image_info[0].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        image_info[0].imageView = temp.base_color_texture.image_view;
        image_info[0].sampler = VulkanUtil::getMipmapSampler(
            m_rhi->m_physical_device,
            m_rhi->m_device,
            temp.base_color_texture.width,
            temp.base_color_texture.height
        );
        image_info[1].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        image_info[1].imageView = temp.specular_texture.image_view;
        image_info[1].sampler = VulkanUtil::getMipmapSampler(
            m_rhi->m_physical_device,
            m_rhi->m_device,
            temp.specular_texture.width,
            temp.specular_texture.height
        );
        image_info[2].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        image_info[2].imageView = temp.normal_texture.image_view;
        image_info[2].sampler = VulkanUtil::getMipmapSampler(
            m_rhi->m_physical_device,
            m_rhi->m_device,
            temp.normal_texture.width,
            temp.normal_texture.height
        );
        image_info[3].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        image_info[3].imageView = temp.emissive_texture.image_view;
        image_info[3].sampler = VulkanUtil::getMipmapSampler(
            m_rhi->m_physical_device,
            m_rhi->m_device,
            temp.emissive_texture.width,
            temp.emissive_texture.height
        );

        VkWriteDescriptorSet descriptorWrites[5];

        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].pNext = NULL;
        descriptorWrites[0].dstSet = temp.material_descriptor_set;
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &material_uniform_buffer_info;

        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].pNext = NULL;
        descriptorWrites[1].dstSet = temp.material_descriptor_set;
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo = &image_info[0];

        descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[2].pNext = NULL;
        descriptorWrites[2].dstSet = temp.material_descriptor_set;
        descriptorWrites[2].dstBinding = 2;
        descriptorWrites[2].dstArrayElement = 0;
        descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[2].descriptorCount = 1;
        descriptorWrites[2].pImageInfo = &image_info[1];

        descriptorWrites[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[3].pNext = NULL;
        descriptorWrites[3].dstSet = temp.material_descriptor_set;
        descriptorWrites[3].dstBinding = 3;
        descriptorWrites[3].dstArrayElement = 0;
        descriptorWrites[3].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[3].descriptorCount = 1;
        descriptorWrites[3].pImageInfo = &image_info[2];

        descriptorWrites[4].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[4].pNext = NULL;
        descriptorWrites[4].dstSet = temp.material_descriptor_set;
        descriptorWrites[4].dstBinding = 4;
        descriptorWrites[4].dstArrayElement = 0;
        descriptorWrites[4].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[4].descriptorCount = 1;
        descriptorWrites[4].pImageInfo = &image_info[3];

        vkUpdateDescriptorSets(m_rhi->m_device, 5, descriptorWrites, 0, nullptr);

        return temp;
    }

    void VulkanRenderResource::cleanup()
    {
        for (auto& pair : m_render_meshes) {
            VulkanMesh& vulkan_mesh = pair.second;
            
            vkDestroyBuffer(m_rhi->m_device, vulkan_mesh.mesh_vertex_buffer, nullptr);
            vkFreeMemory(m_rhi->m_device, vulkan_mesh.mesh_vertex_buffer_memory, nullptr);

            vkDestroyBuffer(m_rhi->m_device, vulkan_mesh.mesh_index_buffer, nullptr);
            vkFreeMemory(m_rhi->m_device, vulkan_mesh.mesh_index_buffer_memory, nullptr);
        }
        for (auto& pair : m_render_materials) {
            VulkanPBRMaterial& vulkan_material = pair.second;

            vkDestroyImageView(m_rhi->m_device, vulkan_material.base_color_texture.image_view, nullptr);
            vkDestroyImage(m_rhi->m_device, vulkan_material.base_color_texture.image, nullptr);
            vkFreeMemory(m_rhi->m_device, vulkan_material.base_color_texture.image_memory, nullptr);

            vkUnmapMemory(m_rhi->m_device, vulkan_material.material_uniform_buffer_memory);
            vkDestroyBuffer(m_rhi->m_device, vulkan_material.material_uniform_buffer, nullptr);
            vkFreeMemory(m_rhi->m_device, vulkan_material.material_uniform_buffer_memory, nullptr);
        }
        m_material_meshes.clear();
        m_render_nodes.clear();
    }

    void VulkanRenderResource::createVertexBuffer(void* vertex_data, uint32_t buffer_size, VulkanMesh &n_mesh)
    {
        VkDeviceSize bufferSize = buffer_size;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        VulkanUtil::createBuffer(
            m_rhi->m_physical_device,
            m_rhi->m_device,
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer,
            stagingBufferMemory
        );

        void* data;
        vkMapMemory(m_rhi->m_device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertex_data, (size_t)bufferSize);
        vkUnmapMemory(m_rhi->m_device, stagingBufferMemory);

        VulkanUtil::createBuffer(
            m_rhi->m_physical_device,
            m_rhi->m_device,
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            n_mesh.mesh_vertex_buffer,
            n_mesh.mesh_vertex_buffer_memory
        );

        VulkanUtil::copyBuffer(m_rhi.get(), stagingBuffer, n_mesh.mesh_vertex_buffer, bufferSize);

        vkDestroyBuffer(m_rhi->m_device, stagingBuffer, nullptr);
        vkFreeMemory(m_rhi->m_device, stagingBufferMemory, nullptr);
    }

    void VulkanRenderResource::createIndexBuffer(void* index_data, uint32_t buffer_size, VulkanMesh& n_mesh)
    {
        VkDeviceSize bufferSize = buffer_size;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        VulkanUtil::createBuffer(
            m_rhi->m_physical_device,
            m_rhi->m_device,
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer,
            stagingBufferMemory
        );

        void* data;
        vkMapMemory(m_rhi->m_device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, index_data, (size_t)bufferSize);
        vkUnmapMemory(m_rhi->m_device, stagingBufferMemory);

        VulkanUtil::createBuffer(
            m_rhi->m_physical_device,
            m_rhi->m_device,
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            n_mesh.mesh_index_buffer,
            n_mesh.mesh_index_buffer_memory
        );

        VulkanUtil::copyBuffer(m_rhi.get(), stagingBuffer, n_mesh.mesh_index_buffer, bufferSize);

        vkDestroyBuffer(m_rhi->m_device, stagingBuffer, nullptr);
        vkFreeMemory(m_rhi->m_device, stagingBufferMemory, nullptr);
    }

}