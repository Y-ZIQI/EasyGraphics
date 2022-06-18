#pragma once

#include "function/render/rhi/vulkan/vulkan_rhi.h"
#include "function/render/rhi/vulkan/vulkan_util.h"
#include "function/render/render_type.h"

#include "resource/res_type/data/mesh_data.h"

//#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

#include <array>
#include <vector>
#include <map>
#include <set>

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

    struct MainPassVertUBO
    {
        alignas(16) glm::mat4 model_matrix;
        alignas(16) glm::mat4 proj_view_matrix;
    };

    //extern const std::vector<VulkanVertex> vertices;
    //extern const std::vector<uint16_t> indices;

    // mesh
    //struct VulkanMesh
    //{
    //    bool enable_vertex_blending;

    //    uint32_t mesh_vertex_count;

    //    VkBuffer      mesh_vertex_position_buffer;
    //    VmaAllocation mesh_vertex_position_buffer_allocation;

    //    VkBuffer      mesh_vertex_varying_enable_blending_buffer;
    //    VmaAllocation mesh_vertex_varying_enable_blending_buffer_allocation;

    //    VkBuffer      mesh_vertex_joint_binding_buffer;
    //    VmaAllocation mesh_vertex_joint_binding_buffer_allocation;

    //    VkDescriptorSet mesh_vertex_blending_descriptor_set;

    //    VkBuffer      mesh_vertex_varying_buffer;
    //    VmaAllocation mesh_vertex_varying_buffer_allocation;

    //    uint32_t mesh_index_count;

    //    VkBuffer      mesh_index_buffer;
    //    VmaAllocation mesh_index_buffer_allocation;
    //};

    struct MeshPerMaterialUniformBufferObject
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

    struct RenderResourceInitInfo
    {
        std::shared_ptr<VulkanRHI> rhi;
    };

    class VulkanRenderResource
    {
    public:
        VulkanRenderResource() {};
        ~VulkanRenderResource() {};

        void initialize(RenderResourceInitInfo init_info);

        VulkanMesh createMesh(RenderMeshData mesh_data);
        VulkanPBRMaterial createPBRMaterial(RenderMaterialData material_data);

        RenderMeshData loadMeshData(const MeshData& source);
        std::shared_ptr<TextureData> loadTexture(const std::string& file, bool is_srgb);
        RenderMaterialData loadMaterialData(const std::string& tex_file);

        void cleanup();

    private:
        void createVertexBuffer(void* vertex_data, uint32_t buffer_size, VulkanMesh &n_mesh);
        void createIndexBuffer(void* index_data, uint32_t buffer_size, VulkanMesh &n_mesh);

    public:
        std::shared_ptr<VulkanRHI> m_rhi;
        std::map<uint32_t, VulkanMesh> m_render_meshes;
        std::map<uint32_t, VulkanPBRMaterial> m_render_materials;

        std::map<uint32_t, std::set<uint32_t>> m_material_meshes;

        // descriptor set layout in main camera pass will be used when uploading resource
        const VkDescriptorSetLayout* m_mesh_descriptor_set_layout = nullptr;
        const VkDescriptorSetLayout* m_material_descriptor_set_layout = nullptr;
    };
}