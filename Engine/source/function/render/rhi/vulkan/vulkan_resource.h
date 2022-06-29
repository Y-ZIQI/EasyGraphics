#pragma once

#include "function/render/rhi/vulkan/vulkan_rhi.h"
#include "function/render/rhi/vulkan/vulkan_util.h"
#include "function/render/render_type.h"
#include "function/render/rhi/vulkan/vulkan_types.h"

#include "resource/res_type/data/mesh_data.h"

#include <vector>
#include <map>
#include <set>

namespace Eagle
{
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

        void cleanup();

    private:
        void createVertexBuffer(void* vertex_data, uint32_t buffer_size, VulkanMesh &n_mesh);
        void createIndexBuffer(void* index_data, uint32_t buffer_size, VulkanMesh &n_mesh);

    public:
        std::shared_ptr<VulkanRHI> m_rhi;

        //std::vector<VulkanMeshNode> m_render_nodes;
        std::map<uint32_t, std::map<uint32_t, VulkanMeshNode>> m_render_nodes;

        std::map<uint32_t, VulkanMesh> m_render_meshes;
        std::map<uint32_t, VulkanPBRMaterial> m_render_materials;
        std::map<uint32_t, std::set<uint32_t>> m_material_meshes;

        // descriptor set layout in main camera pass will be used when uploading resource
        const VkDescriptorSetLayout* m_mesh_descriptor_set_layout = nullptr;
        const VkDescriptorSetLayout* m_material_descriptor_set_layout = nullptr;
    };
}