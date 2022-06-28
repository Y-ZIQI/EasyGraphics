#include "function/render/rhi/vulkan/vulkan_resource.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <vector>

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

    VulkanPBRMaterial VulkanRenderResource::createPBRMaterial(RenderMaterialData material_data)
    {
        uint32_t width = material_data.m_base_color_texture->m_width, height = material_data.m_base_color_texture->m_height;

        VulkanPBRMaterial temp;
        temp.desc = { width, height };

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        VkDeviceSize imageSize = width * height * 4;

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
        memcpy(data, material_data.m_base_color_texture->m_pixels, static_cast<size_t>(imageSize));
        vkUnmapMemory(m_rhi->m_device, stagingBufferMemory);

        uint32_t mip_levels = std::floor(std::log2(std::max(width, height))) + 1;

        VulkanUtil::createImage(
            m_rhi->m_physical_device,
            m_rhi->m_device,
            width,
            height,
            VK_FORMAT_R8G8B8A8_SRGB, 
            VK_IMAGE_TILING_OPTIMAL, 
            VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
            temp.base_color_texture_image,
            temp.base_color_texture_image_memory,
            0,
            1,
            mip_levels
        );

        VulkanUtil::transitionImageLayout(
            m_rhi.get(), 
            temp.base_color_texture_image,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            mip_levels,
            VK_IMAGE_ASPECT_COLOR_BIT
        );
        VulkanUtil::copyBufferToImage(
            m_rhi.get(),
            stagingBuffer,
            temp.base_color_texture_image,
            width, 
            height,
            1
        );
        VulkanUtil::transitionImageLayout(
            m_rhi.get(),
            temp.base_color_texture_image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            1,
            mip_levels,
            VK_IMAGE_ASPECT_COLOR_BIT
        );

        vkDestroyBuffer(m_rhi->m_device, stagingBuffer, nullptr);
        vkFreeMemory(m_rhi->m_device, stagingBufferMemory, nullptr);

        VulkanUtil::generateMipmaps(m_rhi.get(), temp.base_color_texture_image, VK_FORMAT_R8G8B8A8_SRGB, width, height, 1, mip_levels);

        temp.base_color_image_view = VulkanUtil::createImageView(
            m_rhi->m_device,
            temp.base_color_texture_image,
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_ASPECT_COLOR_BIT,
            VK_IMAGE_VIEW_TYPE_2D,
            1,
            mip_levels
        );

        // TODO: initialize material uniform buffer
        VkDeviceSize bufferSize = sizeof(MeshPerMaterialUniformBufferObject);
        for (size_t i = 0; i < m_rhi->m_max_frames_in_flight; i++) {
            VulkanUtil::createBuffer(
                m_rhi->m_physical_device,
                m_rhi->m_device,
                bufferSize,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                temp.material_uniform_buffer,
                temp.material_uniform_buffer_memory
            );
        }

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
        material_uniform_buffer_info.range = sizeof(MeshPerMaterialUniformBufferObject);
        material_uniform_buffer_info.buffer = temp.material_uniform_buffer;

        VkDescriptorImageInfo base_color_image_info = {};
        base_color_image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        base_color_image_info.imageView = temp.base_color_image_view;
        base_color_image_info.sampler = VulkanUtil::getMipmapSampler(
            m_rhi->m_physical_device,
            m_rhi->m_device,
            width,
            height
        );

        VkWriteDescriptorSet descriptorWrites[2];

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
        descriptorWrites[1].pImageInfo = &base_color_image_info;

        vkUpdateDescriptorSets(m_rhi->m_device, 2, descriptorWrites, 0, nullptr);

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

            vkDestroyImageView(m_rhi->m_device, vulkan_material.base_color_image_view, nullptr);
            vkDestroyImage(m_rhi->m_device, vulkan_material.base_color_texture_image, nullptr);
            vkFreeMemory(m_rhi->m_device, vulkan_material.base_color_texture_image_memory, nullptr);
        }
    }

    RenderMeshData VulkanRenderResource::loadMeshData(const MeshData& source)
    {
        RenderMeshData ret;

        // vertex buffer
        size_t vertex_size = source.vertex_buffer.size() * sizeof(MeshVertexDataDefinition);
        ret.m_static_mesh_data.m_vertex_buffer = std::make_shared<BufferData>(vertex_size);
        MeshVertexDataDefinition* vertex =
            (MeshVertexDataDefinition*)ret.m_static_mesh_data.m_vertex_buffer->m_data;
        for (size_t i = 0; i < source.vertex_buffer.size(); i++)
        {
            /*vertex[i].x = source.vertex_buffer[i].x;
            vertex[i].y = source.vertex_buffer[i].y;
            vertex[i].z = source.vertex_buffer[i].z;
            vertex[i].r = source.vertex_buffer[i].r;
            vertex[i].g = source.vertex_buffer[i].g;
            vertex[i].b = source.vertex_buffer[i].b;
            vertex[i].u = source.vertex_buffer[i].u;
            vertex[i].v = source.vertex_buffer[i].v;*/

            vertex[i].x = source.vertex_buffer[i].pos.x;
            vertex[i].y = source.vertex_buffer[i].pos.y;
            vertex[i].z = source.vertex_buffer[i].pos.z;
            vertex[i].r = source.vertex_buffer[i].color.r;
            vertex[i].g = source.vertex_buffer[i].color.g;
            vertex[i].b = source.vertex_buffer[i].color.b;
            vertex[i].u = source.vertex_buffer[i].tex_coord.x;
            vertex[i].v = source.vertex_buffer[i].tex_coord.y;
        }

        // index buffer
        size_t index_size = source.index_buffer.size() * sizeof(MeshIndexDataDefinition);
        ret.m_static_mesh_data.m_index_buffer = std::make_shared<BufferData>(index_size);
        MeshIndexDataDefinition* index = (MeshIndexDataDefinition*)ret.m_static_mesh_data.m_index_buffer->m_data;
        for (size_t i = 0; i < source.index_buffer.size(); i++)
        {
            index[i] = static_cast<MeshIndexDataDefinition>(source.index_buffer[i]);
        }

        return ret;
    }

    std::shared_ptr<TextureData> VulkanRenderResource::loadTexture(const std::string& file, bool is_srgb, bool vflip)
    {
        std::shared_ptr<TextureData> texture = std::make_shared<TextureData>();

        int iw, ih, n;
        stbi_set_flip_vertically_on_load(vflip);
        texture->m_pixels = stbi_load(file.c_str(), &iw, &ih, &n, 4);

        if (!texture->m_pixels)
            return nullptr;

        texture->m_width = iw;
        texture->m_height = ih;
        texture->m_format = (is_srgb) ? EAGLE_PIXEL_FORMAT::EAGLE_PIXEL_FORMAT_R8G8B8A8_SRGB :
            EAGLE_PIXEL_FORMAT::EAGLE_PIXEL_FORMAT_R8G8B8A8_UNORM;
        texture->m_depth = 1;
        texture->m_array_layers = 1;
        texture->m_mip_levels = 1;
        texture->m_type = EAGLE_IMAGE_TYPE::EAGLE_IMAGE_TYPE_2D;

        return texture;
    }

    RenderMaterialData VulkanRenderResource::loadMaterialData(const std::string& tex_file, bool vflip)
    {
        RenderMaterialData ret;
        ret.m_base_color_texture = loadTexture(tex_file, true, vflip);
        return ret;
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