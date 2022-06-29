#pragma once
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

#include <array>

namespace Eagle
{
    struct VulkanVertex {
        glm::vec3 pos;
        glm::vec3 color;
        glm::vec2 texCoord;

        static VkVertexInputBindingDescription getBindingDescription() {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(VulkanVertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            return bindingDescription;
        }

        static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
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

            return attributeDescriptions;
        }
    };

    struct MeshPerFrameUBO
    {
        alignas(16) glm::mat4 model_matrix;
        alignas(16) glm::mat4 proj_view_matrix;
    };

    struct MeshPerDrawUBO
    {
        alignas(16) glm::mat4 model_matrix;
    };

    struct MeshPerMaterialUBO
    {
        glm::vec4 baseColorFactor{ 0.0f, 0.0f, 0.0f, 0.0f };

        /*float metallicFactor = 0.0f;
        float roughnessFactor = 0.0f;
        float normalScale = 0.0f;
        float occlusionStrength = 0.0f;

        glm::vec3  emissiveFactor = { 0.0f, 0.0f, 0.0f };
        uint32_t is_blend = 0;
        uint32_t is_double_sided = 0;*/
    };

    struct VulkanMesh
    {
        VkDescriptorSet mesh_vertex_blending_descriptor_set;

        uint32_t mesh_vertex_count;
        VkBuffer mesh_vertex_buffer;
        VkDeviceMemory mesh_vertex_buffer_memory;

        uint32_t mesh_index_count;
        VkBuffer mesh_index_buffer;
        VkDeviceMemory mesh_index_buffer_memory;
    };

    // material
    struct VulkanPBRMaterialDesc {
        uint32_t width;
        uint32_t height;
    };
    struct VulkanPBRMaterial
    {
        VulkanPBRMaterialDesc desc;

        VkImage         base_color_texture_image;
        VkDeviceMemory  base_color_texture_image_memory;
        VkImageView     base_color_image_view;

        //VkImage       base_color_texture_image = VK_NULL_HANDLE;
        //VkImageView   base_color_image_view = VK_NULL_HANDLE;
        //VmaAllocation base_color_image_allocation;

        //VkImage       metallic_roughness_texture_image = VK_NULL_HANDLE;
        //VkImageView   metallic_roughness_image_view = VK_NULL_HANDLE;
        //VmaAllocation metallic_roughness_image_allocation;

        //VkImage       normal_texture_image = VK_NULL_HANDLE;
        //VkImageView   normal_image_view = VK_NULL_HANDLE;
        //VmaAllocation normal_image_allocation;

        //VkImage       occlusion_texture_image = VK_NULL_HANDLE;
        //VkImageView   occlusion_image_view = VK_NULL_HANDLE;
        //VmaAllocation occlusion_image_allocation;

        //VkImage       emissive_texture_image = VK_NULL_HANDLE;
        //VkImageView   emissive_image_view = VK_NULL_HANDLE;
        //VmaAllocation emissive_image_allocation;

        VkBuffer        material_uniform_buffer;
        VkDeviceMemory  material_uniform_buffer_memory;

        VkDescriptorSet material_descriptor_set;
    };

    struct VulkanMeshNode
    {
        glm::mat4   model_matrix;
        uint32_t    mesh_id;
        uint32_t    material_id;
    };
}