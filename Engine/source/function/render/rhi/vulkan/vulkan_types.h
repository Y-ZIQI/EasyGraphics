#pragma once
#include <vulkan/vulkan.h>

#include <glm/glm.hpp>

#include <array>

namespace Eagle
{
    enum class EAGLE_MATERIAL_FLAGS : uint32_t
    {
        EAGLE_MATERIAL_FLAGS_BASECOLOR = 0x00000001,
        EAGLE_MATERIAL_FLAGS_SPECULAR = 0x00000002,
        EAGLE_MATERIAL_FLAGS_NORMAL = 0x00000004,
        EAGLE_MATERIAL_FLAGS_EMISSIVE = 0x00000008
    };

    struct VulkanVertex {
        glm::vec3 pos;
        glm::vec3 normal;
        glm::vec2 texCoord;
        glm::vec3 tangent;
        glm::vec3 bitangent;

        static VkVertexInputBindingDescription getBindingDescription() {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(VulkanVertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            return bindingDescription;
        }

        static std::array<VkVertexInputAttributeDescription, 5> getAttributeDescriptions() {
            std::array<VkVertexInputAttributeDescription, 5> attributeDescriptions{};
            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(VulkanVertex, pos);
            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(VulkanVertex, normal);
            attributeDescriptions[2].binding = 0;
            attributeDescriptions[2].location = 2;
            attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[2].offset = offsetof(VulkanVertex, texCoord);
            attributeDescriptions[3].binding = 0;
            attributeDescriptions[3].location = 3;
            attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[3].offset = offsetof(VulkanVertex, tangent);
            attributeDescriptions[4].binding = 0;
            attributeDescriptions[4].location = 4;
            attributeDescriptions[4].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[4].offset = offsetof(VulkanVertex, bitangent);

            return attributeDescriptions;
        }
    };

    // Vulkan UBO

    struct MeshPerFrameUBO
    {
        alignas(16) glm::mat4 proj_view_matrix;
        alignas(16) glm::vec3 camera_pos;
    };

    struct MeshPerDrawUBO
    {
        alignas(16) glm::mat4 model_matrix;
        alignas(16) glm::mat4 normal_matrix;
    };

    struct MeshPerMaterialUBO
    {
        alignas(4) uint32_t flags = 0;
        alignas(16) glm::vec4 baseColorFactor{ 1.0f, 1.0f, 1.0f, 0.0f };
        alignas(16) glm::vec4 specularFactor{ 0.0f, 1.0f, 0.0f, 0.0f };
        alignas(16) glm::vec4 normalFactor{ 0.0f, 0.0f, 0.0f, 0.0f };
        alignas(16) glm::vec4 emissiveFactor{ 0.0f, 0.0f, 0.0f, 0.0f };
        /*uint32_t is_blend = 0;
        uint32_t is_double_sided = 0;*/
    };

    struct DirectionalLightUBO
    {
        alignas(16) glm::vec4 intensity{ 0.0f, 0.0f, 0.0f, 0.0f };
        alignas(16) glm::vec4 direction{ 0.0f, 0.0f, 0.0f, 0.0f };
        alignas(16) glm::vec4 status{ 512.0f, 0.0f, 0.0f, 0.0f };
        alignas(16) glm::mat4 proj_view_matrix;
    };

    struct ShadingPerFrameUBO
    {
        alignas(16) glm::mat4 proj_view_matrix;
        alignas(16) glm::vec3 camera_pos;
        DirectionalLightUBO dir_light;
    };

    struct ShadowPerFrameUBO
    {
        glm::mat4 proj_view_matrix;
    };

    struct ShadowPerDrawUBO
    {
        glm::mat4 model_matrix;
    };

    // Vulkan resource

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

    struct VulkanTexture
    {
        VkImage         image;
        VkDeviceMemory  image_memory;
        VkImageView     image_view;
        uint32_t        width;
        uint32_t        height;
        uint32_t        mip_levels;
    };

    struct VulkanPBRMaterial
    {
        VulkanTexture   base_color_texture;
        VulkanTexture   specular_texture;
        VulkanTexture   normal_texture;
        VulkanTexture   emissive_texture;

        VkBuffer        material_uniform_buffer;
        VkDeviceMemory  material_uniform_buffer_memory;
        void*           material_uniform_memory_pointer;

        VkDescriptorSet material_descriptor_set;
    };

    struct VulkanMeshNode
    {
        glm::mat4   model_matrix;
        glm::mat3   normal_matrix;
        uint32_t    mesh_id;
        uint32_t    material_id;
    };
}