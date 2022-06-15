#include "function/render/render_resource.h"

namespace Eagle
{
    const MeshData mesh = { 
    {
        {-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f},
        {0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f},
        {0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f},
        {-0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},
        {-0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f},
        {0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
        {0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f},
        {-0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f}
    },
    {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4
    }
    };

    const std::string texture_file = "../Engine/resources/textures/texture.jpg";

    void RenderResource::uploadMeshRenderResoucre(std::shared_ptr<RHI> rhi, RenderMeshData mesh_data, RenderMaterialData material_data)
    {
        auto n_mesh = createMesh(mesh_data);
        m_render_meshes[0] = n_mesh;
        auto n_material = createPBRMaterial(material_data);
        m_render_materials[0] = n_material;
    }

}